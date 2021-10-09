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

protected:

	ADAGPlayerController* GetPlayerControllerById(int32 PlayerId);

protected:

	TArray<ADAGPlayerController*> PlayerControllerList;
};
