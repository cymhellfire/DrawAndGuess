// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/DAGStandardGameMode.h"
#include "Actors/DrawingCanvas.h"
#include "DrawAndGuess/DrawAndGuess.h"
#include "Framework/DAGGameInstance.h"
#include "Framework/DAGGameUserSettings.h"
#include "Framework/DAGPlayerController.h"
#include "Framework/DAGPlayerState.h"
#include "Framework/WordPool.h"
#include "GameFramework/PlayerState.h"
#include "GameMode/DAGStandardGameState.h"
#include "GameMode/DAGStandardPlayerController.h"

ADAGStandardGameMode::ADAGStandardGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentGamePhase = EStandardGameModePhase::SGMP_Waiting;
	DrawingTimePerRound = 30;
	ChooseWordTimePerRound = 10;
	MaxDrawingRounds = 3;
	FinishedRound = 0;

	PlayerControllerClass = ADAGStandardPlayerController::StaticClass();
}

void ADAGStandardGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Switch to pending game phase
	if (PendingGamePhase != CurrentGamePhase)
	{
		SwitchToPhase(PendingGamePhase);
	}
}

void ADAGStandardGameMode::ChooseWordByIndex(int32 Index)
{
	// Clear timer
	GetWorldTimerManager().ClearTimer(ChooseWordTimerHandle);

	if (Index < 0 || Index >= CandidateWords.Num())
	{
		return;
	}

	CurrentWord = CandidateWords[Index];
	OnWordChosen();
}

void ADAGStandardGameMode::RestartGame()
{
	for (ADAGPlayerController* PlayerController : PlayerControllerList)
	{
		// Notify all players that game restarted
		PlayerController->ClientReceiveGameRestart();

		// Clear all player's drawing score
		if (ADAGPlayerState* PlayerState = PlayerController->GetPlayerState<ADAGPlayerState>())
		{
			PlayerState->SetDrawScore(0);
		}
	}

	FinishedRound = 0;

	// Set game phase to game start
	SetNextPhase(SGMP_GameStart);
}

void ADAGStandardGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Collect all canvas
	for (TActorIterator<ADrawingCanvas> Iter(GetWorld()); Iter; ++Iter)
	{
		CanvasList.AddUnique(*Iter);
	}
}

void ADAGStandardGameMode::SetNextPhase(EStandardGameModePhase NextPhase)
{
	PendingGamePhase = NextPhase;
}

void ADAGStandardGameMode::SwitchToPhase(EStandardGameModePhase NewPhase)
{
	switch (NewPhase)
	{
	case SGMP_Waiting:
		break;
	case SGMP_GameStart:
		OnGameStarted();
		break;
	case SGMP_RoundStart:
		OnRoundStarted();
		break;
	case SGMP_ChooseWord:
		OnChooseWord();
		break;
	case SGMP_PlayerDrawing:
		OnPlayerDrawing();
		break;
	case SGMP_RoundEnd:
		OnRoundEnded();
		break;
	case SGMP_SwitchPlayer:
		OnSwitchPlayer();
		break;
	case SGMP_GameFinish:
		OnGameFinished();
		break;
	default: ;
	}

	CurrentGamePhase = NewPhase;
}

void ADAGStandardGameMode::OnGameStarted()
{
	if (ADAGStandardGameState* StandardGameState = GetGameState<ADAGStandardGameState>())
	{
		StandardGameState->SetCurrentPlayerIndex(0);
	}

	// Load game settings
	if (UDAGGameInstance* GameInstance = Cast<UDAGGameInstance>(GetGameInstance()))
	{
		if (WordPool == nullptr)
		{
			WordPool = NewObject<UWordPool>(this, TEXT("WordPool"));
		}

		if (UDAGGameUserSettings* UserSettings = UDAGGameUserSettings::GetDAGGameUserSettings())
		{
			// Load the word pool
			WordPool->LoadFromFile(UserSettings->GetWordPoolFilePath());

			MaxDrawingRounds = UserSettings->GetMaxDrawingRound();
			UE_LOG(LogInit, Log, TEXT("[GameMode] Set MaxDrawingRound to %d"), MaxDrawingRounds);
			DrawingTimePerRound = UserSettings->GetDrawingTimePerRound();
			UE_LOG(LogInit, Log, TEXT("[GameMode] Set DrawingTimePerRound to %d"), DrawingTimePerRound);
			CandidateWordCount = UserSettings->GetCandidateWordCount();
			UE_LOG(LogInit, Log, TEXT("[GameMode] Set CandidateWordCount to %d"), CandidateWordCount);
		}
	}

	// Start the player round
	SetNextPhase(SGMP_RoundStart);
}

void ADAGStandardGameMode::OnRoundStarted()
{
	CurrentPlayerId = GetPlayerIdByIndex(GetGameState<ADAGStandardGameState>()->GetCurrentPlayerIndex());

	// Notify all players the new round start
	ADAGPlayerState* CurrentPlayerState = GetPlayerControllerById(CurrentPlayerId)->GetPlayerState<ADAGPlayerState>();
	if (CurrentPlayerState)
	{
		for (ADAGPlayerController* PlayerController : PlayerControllerList)
		{
			PlayerController->ClientReceivePlayerRoundStart(CurrentPlayerState);
		}
	}

	if (ADAGGameStateBase* MyGameState = GetGameState<ADAGGameStateBase>())
	{
		MyGameState->ClearDrawerName();
	}

	SetNextPhase(SGMP_ChooseWord);
}

void ADAGStandardGameMode::OnChooseWord()
{
	if (WordPool == nullptr || WordPool->GetWordCount() == 0)
	{
		SetNextPhase(SGMP_GameFinish);
		return;
	}

	// Get word for this round
	CandidateWords.Empty();
	CandidateWords = WordPool->GetRandomWord(CandidateWordCount);

	if (ADAGPlayerController* CurrentPlayerController = GetPlayerControllerById(CurrentPlayerId))
	{
		TArray<FString> CandidateWordStrings;
		for (const FWordInfo* WordInfo : CandidateWords)
		{
			CandidateWordStrings.Add(WordInfo->Word);
		}
		CurrentPlayerController->ClientReceiveCandidateWords(CandidateWordStrings);
	}

	// Reset choose word remain time
	if (ADAGStandardGameState* StandardGameState = GetGameState<ADAGStandardGameState>())
	{
		StandardGameState->SetRemainTime(ChooseWordTimePerRound);
	}

	GetWorldTimerManager().SetTimer(ChooseWordTimerHandle, this, &ADAGStandardGameMode::OnChooseWordTimerTicked,
		GetWorld()->GetWorldSettings()->GetEffectiveTimeDilation(), true);
}

void ADAGStandardGameMode::OnPlayerDrawing()
{
	// Reset drawing remain time
	if (ADAGStandardGameState* StandardGameState = GetGameState<ADAGStandardGameState>())
	{
		StandardGameState->SetRemainTime(DrawingTimePerRound);
	}

	// Setup the drawing handle
	GetWorldTimerManager().SetTimer(DrawingTimerHandle, this, &ADAGStandardGameMode::OnDrawingTimerTicked,
		GetWorld()->GetWorldSettings()->GetEffectiveTimeDilation(), true);
}

void ADAGStandardGameMode::OnRoundEnded()
{
	// Clear the timer handle
	GetWorldTimerManager().ClearTimer(DrawingTimerHandle);

	// Disable player drawing function for all canvas
	for (ADrawingCanvas* DrawingCanvas : CanvasList)
	{
		ForbidPlayerDrawOnCanvas(CurrentPlayerId, DrawingCanvas);
	}

	// Notify current player the word
	if (ADAGPlayerController* CurrentPlayerController = GetPlayerControllerById(CurrentPlayerId))
	{
		if (ADAGPlayerState* CurrentPlayerState = CurrentPlayerController->GetPlayerState<ADAGPlayerState>())
		{
			CurrentPlayerState->SetGameState(EPlayerGameState::PGS_Guessing);
		}
	}

	// Setup a timer before switch player if there is player not get correct answer
	GetWorldTimerManager().SetTimer(RoundEndTimerHandle, this, &ADAGStandardGameMode::OnRoundEndTimerExpired, 5);

	// Show all players the correct answer
	for (ADAGPlayerController* PlayerController : PlayerControllerList)
	{
		PlayerController->ClientReceiveCorrectWord(CurrentWord->Word);
	}
}

void ADAGStandardGameMode::OnRoundEndTimerExpired()
{
	GetWorldTimerManager().ClearTimer(RoundEndTimerHandle);

	// Switch to next player
	SetNextPhase(SGMP_SwitchPlayer);
}

void ADAGStandardGameMode::OnSwitchPlayer()
{
	if (ADAGPlayerController* CurrentPlayerController = GetPlayerControllerById(CurrentPlayerId))
	{
		// Clear the drawing actions
		CurrentPlayerController->ClearDrawingActions();
	}

	ADAGStandardGameState* StandardGameState = GetGameState<ADAGStandardGameState>();
	if (StandardGameState)
	{
		int32 CurrentPlayerIndex = StandardGameState->GetCurrentPlayerIndex();
		CurrentPlayerIndex++;
		if (CurrentPlayerIndex >= PlayerControllerList.Num())
		{
			// All players finished this round
			// Reset the index to start
			CurrentPlayerIndex = 0;

			// Increase finished round counter
			FinishedRound++;

			if (FinishedRound >= MaxDrawingRounds)
			{
				// Finish the game
				SetNextPhase(SGMP_GameFinish);
				return;
			}
		}

		StandardGameState->SetCurrentPlayerIndex(CurrentPlayerIndex);

		// Start a new round
		SetNextPhase(SGMP_RoundStart);
	}
	else
	{
		SetNextPhase(SGMP_GameFinish);
	}
}

void ADAGStandardGameMode::OnGameFinished()
{
	TArray<ADAGPlayerState*> Leaderboard;
	for (ADAGPlayerController* PlayerController : PlayerControllerList)
	{
		if (ADAGPlayerState* MyPlayerState = PlayerController->GetPlayerState<ADAGPlayerState>())
		{
			Leaderboard.Add(MyPlayerState);
		}
	}
	Leaderboard.Sort([](const ADAGPlayerState& PlayerA, const ADAGPlayerState& PlayerB)
	{
		return PlayerA.GetDrawScore() > PlayerB.GetDrawScore();
	});

	TArray<FString> NameArray;
	TArray<int32> ScoreArray;

	for (ADAGPlayerState* PlayerState : Leaderboard)
	{
		NameArray.Add(PlayerState->GetPlayerName());
		ScoreArray.Add(PlayerState->GetDrawScore());
	}

	for (ADAGPlayerController* PlayerController : PlayerControllerList)
	{
		PlayerController->ClientReceiveLeaderboard(NameArray, ScoreArray);
	}
	UE_LOG(LogInit, Log, TEXT("[GameMode] Game finished."));
}

int32 ADAGStandardGameMode::GetPlayerIdByIndex(int32 Index) const
{
	check(Index < PlayerControllerList.Num());

	if (APlayerState* TargetPlayerState = PlayerControllerList[Index]->GetPlayerState<APlayerState>())
	{
		return TargetPlayerState->GetPlayerId();
	}
	else
	{
		UE_LOG(LogDrawAndGuess, Error, TEXT("[StandardGameMode] Invalid game controller with index [%d]"), Index);
	}

	return -1;
}

int32 ADAGStandardGameMode::GetCurrentPlayerId() const
{
	if (ADAGStandardGameState* StandardGameState = GetGameState<ADAGStandardGameState>())
	{
		return StandardGameState->GetCurrentPlayerIndex();
	}

	return -1;
}

void ADAGStandardGameMode::OnWordGuessed(ADAGPlayerController* PlayerController)
{
	GuessedPlayerCount++;

	// Increase player score
	if (ADAGPlayerState* GuesserPlayerState = PlayerController->GetPlayerState<ADAGPlayerState>())
	{
		GuesserPlayerState->AddDrawScore(GetGameState<ADAGStandardGameState>()->GetGuesserGainScore());
	}

	if (ADAGPlayerState* DrawerPlayerState = GetPlayerControllerById(CurrentPlayerId)->GetPlayerState<ADAGPlayerState>())
	{
		DrawerPlayerState->AddDrawScore(GetGameState<ADAGStandardGameState>()->GetDrawerGainScore());
	}

	// Check if all players guessed the word (There should be only one player drawing for now.)
	if (GuessedPlayerCount == PlayerControllerList.Num() - 1)
	{
		SetNextPhase(SGMP_RoundEnd);
	}
}

void ADAGStandardGameMode::OnDrawingTimerTicked()
{
	// Decrease remaining time
	if (ADAGStandardGameState* StandardGameState = GetGameState<ADAGStandardGameState>())
	{
		StandardGameState->SetRemainTime(StandardGameState->GetRemainTime() - 1);

		// Calculate the gain score
		float TimeRatio = StandardGameState->GetRemainTime() / (float)DrawingTimePerRound;
		int32 Score = FMath::CeilToInt(DrawerScoreCurve->GetFloatValue(TimeRatio));
		StandardGameState->SetDrawerGainScore(Score);
		Score = FMath::CeilToInt(GuesserScoreCurve->GetFloatValue(TimeRatio));
		StandardGameState->SetGuesserGainScore(Score);

		// Enter next phase if no remaining time left
		if (StandardGameState->GetRemainTime() <= 0)
		{
			// Enter round finish phase
			SetNextPhase(SGMP_RoundEnd);
		}
	}
}

void ADAGStandardGameMode::OnChooseWordTimerTicked()
{
	// Decrease remaining time
	if (ADAGStandardGameState* StandardGameState = GetGameState<ADAGStandardGameState>())
	{
		StandardGameState->SetRemainTime(StandardGameState->GetRemainTime() - 1);

		// Select a random word if expired
		if (StandardGameState->GetRemainTime() <= 0)
		{
			ChooseWordByIndex(FMath::RandRange(0, CandidateWords.Num() - 1));
		}
	}
}

void ADAGStandardGameMode::OnWordChosen()
{
	if (CurrentWord)
	{
		// Notify current player the word
		if (ADAGPlayerController* CurrentPlayerController = GetPlayerControllerById(CurrentPlayerId))
		{
			CurrentPlayerController->ClientReceiveWord(CurrentWord->Word);

			if (ADAGStandardPlayerController* StandardPlayerController = Cast<ADAGStandardPlayerController>(CurrentPlayerController))
			{
				StandardPlayerController->ClientOnWordChosen();
			}

			// Notify other players the hint
			for (ADAGPlayerController* PlayerController : PlayerControllerList)
			{
				if (PlayerController == CurrentPlayerController)
					continue;

				PlayerController->ClientReceiveHint(FString::Printf(TEXT("%d??????"), CurrentWord->Word.Len()));
			}

			// Start the drawing state
			if (ADAGPlayerState* CurrentPlayerState = CurrentPlayerController->GetPlayerState<ADAGPlayerState>())
			{
				CurrentPlayerState->SetGameState(EPlayerGameState::PGS_Drawing);
			}
		}

		// Enable player drawing function for all canvas
		for (ADrawingCanvas* DrawingCanvas : CanvasList)
		{
			AllowPlayerDrawOnCanvas(CurrentPlayerId, DrawingCanvas);
		}

		GuessedPlayerCount = 0;

		UE_LOG(LogInit, Log, TEXT("[GameMode] Player %d round start."), CurrentPlayerId);

		// Start drawing phase
		SetNextPhase(SGMP_PlayerDrawing);
	}
	else
	{
		UE_LOG(LogInit, Error, TEXT("[GameMode] No valid word to guess."));
		SetNextPhase(SGMP_GameFinish);
	}
}

void ADAGStandardGameMode::PreBroadcastChatMessage(ADAGPlayerController* SourcePlayer, FString& InMessage)
{
	// Check if this message match current word
	if (CurrentWord->IsMatch(InMessage))
	{
		// Replace the message if matched
		InMessage = MaskStringForWord;

		// Notify game
		OnWordGuessed(SourcePlayer);
	}
}

void ADAGStandardGameMode::OnAllPlayerReady()
{
	// Start game after all players ready
	SetNextPhase(SGMP_GameStart);
}
