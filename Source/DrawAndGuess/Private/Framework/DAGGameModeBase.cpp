#include "Framework/DAGGameModeBase.h"
#include "Framework/DAGPlayerController.h"


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
