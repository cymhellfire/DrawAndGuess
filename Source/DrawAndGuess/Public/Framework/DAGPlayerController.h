#pragma once

#include "CoreMinimal.h"
#include "DrawAndGuessType.h"
#include "DAGPlayerController.generated.h"

UCLASS()
class ADAGPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	/** Return the client to main menu gracefully. */
	UFUNCTION(Client, Reliable)
	void ClientReturnToMainMenuWithReason(EReturnToMainMenuReason::Type Reason);

	/** Send just before the host start game. */
	UFUNCTION(Client, Reliable)
	void ClientPreStartGame();

	void HandleReturnToMainMenu();

protected:

	void CleanupSessionOnReturnMain();
};
