#include "Framework/DAGGameStateBase.h"

#include "Framework/DAGGameModeBase.h"
#include "Framework/DAGPlayerController.h"


void ADAGGameStateBase::AddPlayerState(APlayerState* PlayerState)
{
	int32 OrgPlayerCount = PlayerArray.Num();

	Super::AddPlayerState(PlayerState);

	// Check if player count changed
	if (PlayerArray.Num() != OrgPlayerCount)
	{
		OnPlayerStateRegistered.Broadcast(PlayerState);
	}
}

void ADAGGameStateBase::RemovePlayerState(APlayerState* PlayerState)
{
	int32 OrgPlayerCount = PlayerArray.Num();

	Super::RemovePlayerState(PlayerState);

	// Check if player count changed
	if (OrgPlayerCount != PlayerArray.Num())
	{
		OnPlayerStateUnregistered.Broadcast(PlayerState);
	}
}

void ADAGGameStateBase::FinishGameAndReturnToMainMenu()
{
	if (AuthorityGameMode)
	{
		// Invoke the GameMode corresponding method on server side
		ADAGGameModeBase* GameMode = Cast<ADAGGameModeBase>(AuthorityGameMode);
		if (GameMode != nullptr)
		{
			GameMode->RequestFinishAndExitToMainMenu();
		}
	}
	else
	{
		// Only do self job on client side
		ADAGPlayerController* const PlayerController = Cast<ADAGPlayerController>(GetGameInstance()->GetFirstLocalPlayerController());
		if (PlayerController != nullptr)
		{
			PlayerController->HandleReturnToMainMenu();
		}
	}
}
