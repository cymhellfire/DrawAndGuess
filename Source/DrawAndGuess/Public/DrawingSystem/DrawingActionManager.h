#pragma once

#include "CoreMinimal.h"
#include "DrawingSystemCommon.h"
#include "DrawingActionManager.generated.h"

class UDrawingActionBase;

UCLASS()
class DRAWANDGUESS_API UDrawingActionManager : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
public:
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

protected:
	UPROPERTY(Transient)
	TArray<UDrawingActionBase*> DrawingActionStack;
};
