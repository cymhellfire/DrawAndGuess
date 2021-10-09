#include "Framework/DAGGameModeBase.h"

#include "DrawingSystem/DrawingActionManager.h"
#include "Framework/DAGGameStateBase.h"
#include "Framework/DAGPlayerController.h"
#include "Framework/DAGPlayerState.h"
#include "Pawns/DrawingBrush.h"

ADAGGameModeBase::ADAGGameModeBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerControllerClass = ADAGPlayerController::StaticClass();
	PlayerStateClass = ADAGPlayerState::StaticClass();
	GameStateClass = ADAGGameStateBase::StaticClass();
}

void ADAGGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// Record the new player controller
	if (ADAGPlayerController* PlayerController = Cast<ADAGPlayerController>(NewPlayer))
	{
		PlayerControllerList.Add(PlayerController);

		// Spawn drawing action manager for new player
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = PlayerController;
		ADrawingActionManager* NewDrawingActionManager = GetWorld()->SpawnActor<ADrawingActionManager>(SpawnParams);
		if (IsValid(NewDrawingActionManager))
		{
			PlayerController->SetDrawingActionManager(NewDrawingActionManager);
			PlayerController->SetDrawingActionManagerToBrush();
		}
		else
		{
			UE_LOG(LogInit, Error, TEXT("[GameMode] Failed spawn drawing action manager for new player %s."), *NewPlayer->GetName());
		}

		// Upload player info
		PlayerController->ClientUploadPlayerInfo();
	}
}

void ADAGGameModeBase::Logout(AController* Exiting)
{
	// Remove player controller from list
	if (ADAGPlayerController* PlayerController = Cast<ADAGPlayerController>(Exiting))
	{
		PlayerControllerList.Remove(PlayerController);

		// Destroy the corresponding drawing action manager instance
		ADrawingActionManager* DrawingActionManager = PlayerController->GetDrawingActionManager();
		if (IsValid(DrawingActionManager))
		{
			DrawingActionManager->Destroy();
		}
	}

	Super::Logout(Exiting);
}

void ADAGGameModeBase::SendChatMessage(ADAGPlayerController* SourcePlayer, FString Message)
{
	// Get source player name
	FString AuthorName = SourcePlayer->GetPlayerState<APlayerState>()->GetPlayerName();
	// Broadcast the chat message to everyone
	for (ADAGPlayerController* PlayerController : PlayerControllerList)
	{
		PlayerController->ClientReceiveChatMessage(FDAGChatMessage{AuthorName, Message});
	}
}

void ADAGGameModeBase::RequestFinishAndExitToMainMenu()
{
	ADAGPlayerController* LocalPlayerController = nullptr;

	// Notify everyone else to leave game
	for (ADAGPlayerController* PlayerController : PlayerControllerList)
	{
		if (!PlayerController->IsLocalController())
		{
			PlayerController->ClientReturnToMainMenuWithReason(EReturnToMainMenuReason::HostLeft);
		}
		else
		{
			LocalPlayerController = PlayerController;
		}
	}

	if (LocalPlayerController != nullptr)
	{
		LocalPlayerController->HandleReturnToMainMenu();
	}
}

void ADAGGameModeBase::NotifyClientGameWillStart()
{
	// Tell every client game will start
	for (ADAGPlayerController* PlayerController : PlayerControllerList)
	{
		PlayerController->ClientPreStartGame();
	}
}

void ADAGGameModeBase::ForbidPlayerDrawOnCanvas(int32 PlayerId, ADrawingCanvas* TargetCanvas)
{
	check(TargetCanvas);

	ADAGPlayerController* TargetPlayer = GetPlayerControllerById(PlayerId);
	if (TargetPlayer)
	{
		TargetPlayer->MulticastRemoveAcceptCanvas(TargetCanvas);
	}
}

void ADAGGameModeBase::AllowPlayerDrawOnCanvas(int32 PlayerId, ADrawingCanvas* TargetCanvas)
{
	check(TargetCanvas);

	ADAGPlayerController* TargetPlayer = GetPlayerControllerById(PlayerId);
	if (TargetPlayer)
	{
		TargetPlayer->MulticastAddAcceptCanvas(TargetCanvas);
	}
}

ADAGPlayerController* ADAGGameModeBase::GetPlayerControllerById(int32 PlayerId)
{
	ADAGPlayerController* Result = nullptr;

	for (ADAGPlayerController* PlayerController : PlayerControllerList)
	{
		if (APlayerState* MyPlayerState = PlayerController->GetPlayerState<APlayerState>())
		{
			if (MyPlayerState->GetPlayerId() == PlayerId)
			{
				Result = PlayerController;
				break;
			}
		}
	}

	return Result;
}
