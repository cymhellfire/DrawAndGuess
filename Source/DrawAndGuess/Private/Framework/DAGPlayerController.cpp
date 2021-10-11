#include "Framework/DAGPlayerController.h"

#include "Localization.h"
#include "Actors/DrawingCanvas.h"
#include "DrawingSystem/DrawingActionManager.h"
#include "Framework/DAGGameInstance.h"
#include "Framework/DAGGameModeBase.h"
#include "Framework/DAGGameStateBase.h"
#include "Framework/DAGGameUserSettings.h"
#include "Framework/DAGPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "GameMode/DAGStandardGameMode.h"
#include "Pawns/DrawingBrush.h"

void ADAGPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ADAGPlayerController, DrawingActionManager, COND_OwnerOnly);
}

void ADAGPlayerController::ClientPreStartGame_Implementation()
{
	// Show loading screen locally
	if (UDAGGameInstance* GameInstance = Cast<UDAGGameInstance>(GetGameInstance()))
	{
		GameInstance->ShowLoadingScreen();
	}
}

void ADAGPlayerController::ClientReturnToMainMenuWithReason_Implementation(EReturnToMainMenuReason::Type Reason)
{
	if (UGameInstance* const GameInstance = GetGameInstance())
	{
		// Remove the error handler first
		if (UDAGGameInstance* DAGGameInstance = Cast<UDAGGameInstance>(GameInstance))
		{
			DAGGameInstance->RemoveNetworkFailureHandler();
		}

		GameInstance->ReturnToMainMenu();

		// Add reason message
		if (UDAGGameInstance* DAGGameInstance = Cast<UDAGGameInstance>(GameInstance))
		{
#if UE_EDITOR
			const FString ErrorContextString = UEnum::GetDisplayValueAsText(Reason).ToString();
#else
			const FString ErrorContextString = FString::Printf(TEXT("NetError_%s"),*UEnum::GetDisplayValueAsText(Reason).ToString());
#endif
			UE_LOG(LogDrawAndGuess, Log, TEXT("DisplayValueAsText: %s"), *ErrorContextString);
			
			DAGGameInstance->AddPendingMessage(FText::FromStringTable(ST_UI_LOC, TEXT("NetErrorTitle")),
				FText::FromStringTable(ST_UI_LOC, ErrorContextString));
		}
	}
	else
	{
		UWorld* const World = GetWorld();
		GEngine->HandleDisconnect(World, World->GetNetDriver());
	}
}

void ADAGPlayerController::ClientUploadPlayerInfo_Implementation()
{
	if (GEngine != nullptr)
	{
		if (UDAGGameUserSettings* UserSettings = Cast<UDAGGameUserSettings>(GEngine->GetGameUserSettings()))
		{
			ServerReceivePlayerInfo(UserSettings->GetPlayerName());
		}
	}
}

void ADAGPlayerController::ServerReceivePlayerInfo_Implementation(const FString& InPlayerName)
{
	FString UniqueName = InPlayerName;
	if (ADAGGameStateBase* GameStateBase = Cast<ADAGGameStateBase>(GetWorld()->GetGameState()))
	{
		UniqueName = GameStateBase->GetUniquePlayerName(InPlayerName);
	}

	// Sync player name with uploaded data
	if (APlayerState* MyPlayerState = GetPlayerState<APlayerState>())
	{
		MyPlayerState->SetPlayerName(UniqueName);
	}

	if (ADAGGameModeBase* MyGameMode = Cast<ADAGGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		MyGameMode->MarkPlayerAsReady(this);
	}
}

void ADAGPlayerController::HandleReturnToMainMenu()
{
	CleanupSessionOnReturnMain();
}

void ADAGPlayerController::ServerSetPlayerName_Implementation(const FString& PlayerName)
{
	FString UniqueName = PlayerName;
	if (ADAGGameStateBase* GameStateBase = Cast<ADAGGameStateBase>(GetWorld()->GetGameState()))
	{
		UniqueName = GameStateBase->GetUniquePlayerName(PlayerName);
	}

	if (ADAGPlayerState* MyPlayerState = GetPlayerState<ADAGPlayerState>())
	{
		MyPlayerState->SetPlayerName(UniqueName);
	}

	if (ADAGGameModeBase* MyGameMode = Cast<ADAGGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		MyGameMode->MarkPlayerAsReady(this);
	}
}

void ADAGPlayerController::ServerSetLobbyState_Implementation(EPlayerLobbyState NewState)
{
	if (ADAGPlayerState* MyPlayerState = GetPlayerState<ADAGPlayerState>())
	{
		MyPlayerState->SetLobbyState(NewState);
	}
}

void ADAGPlayerController::SetDrawingActionManager(ADrawingActionManager* NewDrawingActonManager)
{
	if (NewDrawingActonManager != DrawingActionManager)
	{
		DrawingActionManager = NewDrawingActonManager;
	}
}

void ADAGPlayerController::SetDrawingActionManagerToBrush()
{
	// Set drawing action manager if matches
	if (ADrawingBrush* DrawingBrush = Cast<ADrawingBrush>(GetPawn()))
	{
		ADrawingActionManager* MyDrawingActionManager = GetDrawingActionManager();
		if (MyDrawingActionManager != nullptr)
		{
			DrawingBrush->ServerSetDrawingActionManager(MyDrawingActionManager);
		}
		else
		{
			UE_LOG(LogInit, Error, TEXT("[PlayerController] No valid drawing action manager %s"), *GetName());
		}
	}
}

void ADAGPlayerController::MulticastAddAcceptCanvas_Implementation(ADrawingCanvas* NewCanvas)
{
	if (ADrawingBrush* ControllingBrush = Cast<ADrawingBrush>(GetPawn()))
	{
		ControllingBrush->AddAcceptCanvas(NewCanvas);
	}
}

void ADAGPlayerController::MulticastRemoveAcceptCanvas_Implementation(ADrawingCanvas* TargetCanvas)
{
	if (ADrawingBrush* ControllingBrush = Cast<ADrawingBrush>(GetPawn()))
	{
		ControllingBrush->RemoveAcceptCanvas(TargetCanvas);
	}
}

void ADAGPlayerController::ServerSendChatMessage_Implementation(const FString& Message)
{
	if (ADAGGameModeBase* DAGGameMode = Cast<ADAGGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		DAGGameMode->SendChatMessage(this, Message);
	}
}

void ADAGPlayerController::ClientReceiveChatMessage_Implementation(FDAGChatMessage Message)
{
	OnReceiveChatMessage.Broadcast(Message);
}

void ADAGPlayerController::ClientReceiveWord_Implementation(const FString& Word)
{
	OnReceiveWord.Broadcast(Word);
}

void ADAGPlayerController::ClientReceiveHint_Implementation(const FString& Hint)
{
	OnReceiveHint.Broadcast(Hint);
}

void ADAGPlayerController::ClientReceiveCandidateWords_Implementation(const TArray<FString>& CandidateWords)
{
	// Invoke blueprint function
	K2_OnReceiveCandidateWords(CandidateWords);
}

void ADAGPlayerController::ClearDrawingActions()
{
	if (ADrawingBrush* DrawingBrush = Cast<ADrawingBrush>(GetPawn()))
	{
		if (GetNetMode() == NM_Client)
		{
			DrawingBrush->ServerClear();
		}
		else
		{
			DrawingBrush->MulticastClear();
		}
	}
}

void ADAGPlayerController::ExecCheckAllPlayerId()
{
	if (AGameStateBase* MyGameState = GetWorld()->GetGameState())
	{
		for (APlayerState* MyPlayerState : MyGameState->PlayerArray)
		{
			UE_LOG(LogInit, Log, TEXT("[CheckPlayerId] PlayerId %d"), MyPlayerState->GetPlayerId());
		}
	}
}

void ADAGPlayerController::ExecSetPlayerDrawOnCanvas(int32 PlayerId, FString CanvasName, bool bCanDraw)
{
	ADAGGameModeBase* MyGameMode = Cast<ADAGGameModeBase>(GetWorld()->GetAuthGameMode());
	if (IsValid(MyGameMode))
	{
		for (TActorIterator<ADrawingCanvas> Iter(GetWorld()); Iter; ++Iter)
		{
			ADrawingCanvas* TargetCanvas = *Iter;
			if (TargetCanvas->GetName() == CanvasName)
			{
				if (bCanDraw)
				{
					MyGameMode->AllowPlayerDrawOnCanvas(PlayerId, TargetCanvas);
				}
				else
				{
					MyGameMode->ForbidPlayerDrawOnCanvas(PlayerId, TargetCanvas);
				}
				break;
			}
		}
	}
}

void ADAGPlayerController::OnPossess(APawn* PawnToPossess)
{
	Super::OnPossess(PawnToPossess);

	SetDrawingActionManagerToBrush();
}

void ADAGPlayerController::CleanupSessionOnReturnMain()
{
	const UWorld* World = GetWorld();
	UDAGGameInstance* GameInstance = World != nullptr ? Cast<UDAGGameInstance>(World->GetGameInstance()) : nullptr;
	if (ensure(GameInstance != nullptr))
	{
		// Let game instance handle the session stuff
		GameInstance->CleanupSessionOnReturnToMenu();
	}
}

void ADAGPlayerController::OnRep_DrawingActionManager()
{
	// Mark owning action manager as local one
	DrawingActionManager->SetLocalFlag(true);
}
