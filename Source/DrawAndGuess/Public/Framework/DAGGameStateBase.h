#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DAGGameStateBase.generated.h"

UCLASS()
class ADAGGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
public:

	void FinishGameAndReturnToMainMenu();
};
