#pragma once

#include "CoreMinimal.h"
#include "DrawingSystemCommon.h"
#include "DrawingActionManager.generated.h"

class UDrawingActionBase;

UCLASS()
class DRAWANDGUESS_API ADrawingActionManager : public AActor
{
	GENERATED_BODY()
public:
	ADrawingActionManager();

	/**
	 * Create a drawing action with given type.
	 *
	 * @return New drawing action instance with given type.
	 */
	UDrawingActionBase* CreateDrawingAction(EDrawingActionType ActionType);

	/**
	 * Submit a drawing action to the manager stack.
	 *
	 * @param DrawingAction			Action to submit.
	 * @return Whether the drawing action is accepted by manager.
	 */
	bool SubmitDrawingAction(UDrawingActionBase* DrawingAction);

	/**
	 * Undo last action.
	 */
	void Undo();

	void SetLocalFlag(bool NewValue) { bIsLocal = NewValue; }

	bool GetLocalFlag() const { return bIsLocal; }

protected:
	UFUNCTION()
	void OnRep_ActionCount();

protected:
	UPROPERTY(ReplicatedUsing=OnRep_ActionCount)
	int32 ActionCount;

	UPROPERTY(VisibleAnywhere)
	bool bIsLocal;

	UPROPERTY(Transient, VisibleAnywhere)
	TArray<UDrawingActionBase*> DrawingActionStack;
};
