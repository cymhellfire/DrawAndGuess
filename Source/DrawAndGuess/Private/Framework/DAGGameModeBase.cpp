#include "Framework/DAGGameModeBase.h"

#include "Framework/DAGGameStateBase.h"
#include "Framework/DAGPlayerController.h"
#include "Framework/DAGPlayerState.h"

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

		// Upload player info
		PlayerController->ClientUploadPlayerInfo();

		// Register the callback for lobby state changes
		if (ADAGPlayerState* PlayerState = PlayerController->GetPlayerState<ADAGPlayerState>())
		{
			PlayerState->OnPlayerLobbyInfoChanged.AddDynamic(this, &ADAGGameModeBase::OnAnyPlayerLobbyInfoChanged);
		}
	}
}

void ADAGGameModeBase::Logout(AController* Exiting)
{
	// Remove player controller from list
	if (ADAGPlayerController* PlayerController = Cast<ADAGPlayerController>(Exiting))
	{
		PlayerControllerList.Remove(PlayerController);

		// Unregister the lobby state callback
		if (ADAGPlayerState* PlayerState = PlayerController->GetPlayerState<ADAGPlayerState>())
		{
			PlayerState->OnPlayerLobbyInfoChanged.RemoveDynamic(this, &ADAGGameModeBase::OnAnyPlayerLobbyInfoChanged);
		}
	}

	Super::Logout(Exiting);
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

void ADAGGameModeBase::NotifyClientRefreshLobbyInfo()
{
	// Tell everyone refresh the lobby info
	for (ADAGPlayerController* PlayerController : PlayerControllerList)
	{
		PlayerController->ClientRefreshLobbyInfo();
	}
}

void ADAGGameModeBase::OnAnyPlayerLobbyInfoChanged()
{
	// Notify all players to get latest lobby state once someone changed state
	NotifyClientRefreshLobbyInfo();
}
