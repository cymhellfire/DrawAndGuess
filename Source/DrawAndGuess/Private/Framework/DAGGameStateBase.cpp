#include "Framework/DAGGameStateBase.h"

#include "Framework/DAGGameModeBase.h"
#include "Framework/DAGPlayerController.h"


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
