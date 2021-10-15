#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DAGGameModeBase.generated.h"

class ADAGPlayerController;
class ADrawingCanvas;

UCLASS()
class ADAGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ADAGGameModeBase(const FObjectInitializer& ObjectInitializer);

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	void SendChatMessage(ADAGPlayerController* SourcePlayer, FString Message);

	void RequestFinishAndExitToMainMenu();

	void NotifyClientGameWillStart();

	UFUNCTION(BlueprintCallable, Category="GameMode")
	TArray<ADAGPlayerController*> GetAllPlayerControllers() const { return PlayerControllerList; }

	void ForbidPlayerDrawOnCanvas(int32 PlayerId, ADrawingCanvas* TargetCanvas);

	void AllowPlayerDrawOnCanvas(int32 PlayerId, ADrawingCanvas* TargetCanvas);

	virtual void MarkPlayerAsReady(ADAGPlayerController* ReadyPlayer);

	UFUNCTION(BlueprintCallable, Category="GameMode")
	virtual void RestartGame() {}

protected:

	ADAGPlayerController* GetPlayerControllerById(int32 PlayerId);

	virtual void PreBroadcastChatMessage(ADAGPlayerController* SourcePlayer, FString& InMessage) {}

	/** Triggered when all human players logged in. */
	virtual void OnAllPlayerReady() {}

protected:

	TArray<ADAGPlayerController*> PlayerControllerList;

	TArray<ADAGPlayerController*> ReadyController;
};
