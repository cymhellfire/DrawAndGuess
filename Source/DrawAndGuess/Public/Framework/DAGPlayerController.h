#pragma once

#include "CoreMinimal.h"
#include "DrawAndGuessType.h"
#include "DAGPlayerController.generated.h"

UCLASS()
class ADAGPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRefreshLobbyInfoRequestedSignature);
	UPROPERTY(BlueprintAssignable, Category="PlayerController")
	FOnRefreshLobbyInfoRequestedSignature OnRefreshLobbyInfoRequested;

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

	UFUNCTION(Client, Reliable)
	void ClientRefreshLobbyInfo();

	void HandleReturnToMainMenu();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="PlayerController")
	void ServerSetLobbyState(EPlayerLobbyState NewState);

protected:

	void CleanupSessionOnReturnMain();
};
