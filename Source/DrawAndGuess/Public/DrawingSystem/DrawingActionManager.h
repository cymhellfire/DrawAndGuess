#pragma once

#include "CoreMinimal.h"
#include "DrawingSystemCommon.h"
#include "DrawingActionManager.generated.h"

class UDrawingActionBase;
class ADrawingCanvas;

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

	/**
	 * Clear all drawing actions.
	 */
	void Clear();

	void SetLocalFlag(bool NewValue) { bIsLocal = NewValue; }

	bool GetLocalFlag() const { return bIsLocal; }

protected:
	UPROPERTY(VisibleAnywhere)
	bool bIsLocal;

	ADrawingCanvas* LastOperatedCanvas;

	TMap<ADrawingCanvas*, TArray<UDrawingActionBase*>> DrawingActionMap;
};
