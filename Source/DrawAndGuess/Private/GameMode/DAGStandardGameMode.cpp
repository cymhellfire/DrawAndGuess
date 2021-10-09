// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/DAGStandardGameMode.h"
#include "Actors/DrawingCanvas.h"
#include "DrawAndGuess/DrawAndGuess.h"
#include "Framework/DAGPlayerController.h"
#include "GameFramework/PlayerState.h"

ADAGStandardGameMode::ADAGStandardGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentGamePhase = EStandardGameModePhase::SGMP_Waiting;
	DrawingTimePerRound = 30;
	MaxDrawingRounds = 3;
	FinishedRound = 0;
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

void ADAGStandardGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Collect all canvas
	for (TActorIterator<ADrawingCanvas> Iter(GetWorld()); Iter; ++Iter)
	{
		CanvasList.AddUnique(*Iter);
	}

	// Test code
	SetNextPhase(SGMP_GameStart);
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
	CurrentPlayerIndex = 0;

	// Start the player round
	SetNextPhase(SGMP_RoundStart);
}

void ADAGStandardGameMode::OnRoundStarted()
{
	CurrentPlayerId = GetPlayerIdByIndex(CurrentPlayerIndex);

	// Enable player drawing function for all canvas
	for (ADrawingCanvas* DrawingCanvas : CanvasList)
	{
		AllowPlayerDrawOnCanvas(CurrentPlayerId, DrawingCanvas);
	}

	UE_LOG(LogInit, Log, TEXT("[GameMode] Player %d round start."), CurrentPlayerId);

	// Start drawing phase
	SetNextPhase(SGMP_PlayerDrawing);
}

void ADAGStandardGameMode::OnPlayerDrawing()
{
	// Setup the drawing handle
	GetWorldTimerManager().SetTimer(DrawingTimerHandle, this, &ADAGStandardGameMode::OnDrawingTimerExpired,
		DrawingTimePerRound);
}

void ADAGStandardGameMode::OnRoundEnded()
{
	// Disable player drawing function for all canvas
	for (ADrawingCanvas* DrawingCanvas : CanvasList)
	{
		ForbidPlayerDrawOnCanvas(CurrentPlayerId, DrawingCanvas);
	}

	// Switch to next player
	SetNextPhase(SGMP_SwitchPlayer);
}

void ADAGStandardGameMode::OnSwitchPlayer()
{
	CurrentPlayerIndex++;
	if (CurrentPlayerIndex >= PlayerControllerList.Num())
	{
		// All players finished this round
		if (FinishedRound >= MaxDrawingRounds)
		{
			// Finish the game
			SetNextPhase(SGMP_GameFinish);
			return;
		}

		// Reset the index to start
		CurrentPlayerIndex = 0;

		// Increase finished round counter
		FinishedRound++;
	}

	// Start a new round
	SetNextPhase(SGMP_RoundStart);
}

void ADAGStandardGameMode::OnGameFinished()
{
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

void ADAGStandardGameMode::OnDrawingTimerExpired()
{
	// Clear the handle
	GetWorldTimerManager().ClearTimer(DrawingTimerHandle);

	// Enter round finish phase
	SetNextPhase(SGMP_RoundEnd);
}
