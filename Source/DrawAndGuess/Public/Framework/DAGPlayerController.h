#pragma once

#include "CoreMinimal.h"
#include "DrawAndGuessType.h"
#include "DAGPlayerController.generated.h"

class ADrawingActionManager;
class ADrawingCanvas;

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
	void ServerSetPlayerName(const FString& PlayerName);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="PlayerController")
	void ServerSetLobbyState(EPlayerLobbyState NewState);

	void SetDrawingActionManager(ADrawingActionManager* NewDrawingActionManager);

	ADrawingActionManager* GetDrawingActionManager() const { return DrawingActionManager; }

	void SetDrawingActionManagerToBrush();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAddAcceptCanvas(ADrawingCanvas* NewCanvas);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRemoveAcceptCanvas(ADrawingCanvas* TargetCanvas);

	UFUNCTION(Exec)
	void ExecCheckAllPlayerId();

	UFUNCTION(Exec)
	void ExecSetPlayerDrawOnCanvas(int32 PlayerId, FString CanvasName, bool bCanDraw);

protected:
	virtual void OnPossess(APawn* PawnToPossess) override;

	void CleanupSessionOnReturnMain();

	UFUNCTION()
	void OnRep_DrawingActionManager();

protected:

	UPROPERTY(ReplicatedUsing=OnRep_DrawingActionManager, VisibleAnywhere)
	ADrawingActionManager* DrawingActionManager;
};
