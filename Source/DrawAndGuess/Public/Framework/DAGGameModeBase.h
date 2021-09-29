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
	void RequestFinishAndExitToMainMenu();

	void NotifyClientGameWillStart();

protected:

	TArray<ADAGPlayerController*> PlayerControllerList;
};
