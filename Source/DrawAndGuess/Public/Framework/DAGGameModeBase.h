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

	void NotifyClientRefreshLobbyInfo();

	UFUNCTION(BlueprintCallable, Category="GameMode")
	TArray<ADAGPlayerController*> GetAllPlayerControllers() const { return PlayerControllerList; }

protected:
	UFUNCTION()
	void OnAnyPlayerLobbyInfoChanged();

protected:

	TArray<ADAGPlayerController*> PlayerControllerList;
};
