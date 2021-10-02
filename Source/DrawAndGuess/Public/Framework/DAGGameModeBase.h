#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DAGGameModeBase.generated.h"

class ADAGPlayerController;

UCLASS()
class ADAGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ADAGGameModeBase(const FObjectInitializer& ObjectInitializer);

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	void RequestFinishAndExitToMainMenu();

	void NotifyClientGameWillStart();

	UFUNCTION(BlueprintCallable, Category="GameMode")
	TArray<ADAGPlayerController*> GetAllPlayerControllers() const { return PlayerControllerList; }

protected:

	TArray<ADAGPlayerController*> PlayerControllerList;
};
