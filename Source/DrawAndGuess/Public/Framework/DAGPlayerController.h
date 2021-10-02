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

	UFUNCTION(Client, Reliable)
	void ClientUploadPlayerInfo();

	UFUNCTION(Server, Reliable)
	void ServerReceivePlayerInfo(const FString& InPlayerName);

	void HandleReturnToMainMenu();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="PlayerController")
	void ServerSetLobbyState(EPlayerLobbyState NewState);

protected:

	void CleanupSessionOnReturnMain();
};
