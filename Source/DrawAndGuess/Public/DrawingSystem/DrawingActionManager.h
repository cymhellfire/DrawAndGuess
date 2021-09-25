#pragma once

#include "CoreMinimal.h"
#include "DrawingSystemCommon.h"
#include "DrawingActionManager.generated.h"

class UDrawingActionBase;

UCLASS()
class DRAWANDGUESS_API UDrawingActionManager : public UObject
{
	GENERATED_BODY()
public:
	static UDrawingActionManager* GetInstance();

	void Initialize();

	/**
	 * Create a drawing action with given type.
	 *
	 * @return New drawing action instance with given type.
	 */
	UDrawingActionBase* CreateDrawingAction(EDrawingActionType ActionType);

	/**
	 * Undo last action.
	 */
	void Undo();

protected:

	void OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);

protected:
	UPROPERTY(Transient)
	TArray<UDrawingActionBase*> DrawingActionStack; 

	static UDrawingActionManager* Instance;

	FDelegateHandle WorldCleanupDelegateHandle;
};
