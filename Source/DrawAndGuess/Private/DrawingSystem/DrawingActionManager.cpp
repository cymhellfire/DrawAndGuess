#include "DrawingSystem/DrawingActionManager.h"

#include "Actors/DrawingCanvas.h"
#include "DrawingSystem/DrawingActionBase.h"
#include "DrawingSystem/DrawingAction_Pencil.h"

UDrawingActionBase* UDrawingActionManager::CreateDrawingAction(EDrawingActionType ActionType)
{
	UDrawingActionBase* NewAction = nullptr;
	switch(ActionType)
	{
	case DAT_Pencil:
		NewAction = NewObject<UDrawingAction_Pencil>(GetWorld());
		break;
	default: ;
	}

	if (NewAction != nullptr)
	{
		DrawingActionStack.Add(NewAction);
	}

	return NewAction;
}

void UDrawingActionManager::Undo()
{
	if (DrawingActionStack.Num() == 0)
	{
		UE_LOG(LogInit, Log, TEXT("[DrawingActionManager] Nothing to undo."));
		return;
	}

	// Pop the last action
	UDrawingActionBase* UndoAction = DrawingActionStack.Pop();
	ADrawingCanvas* UndoCanvas = UndoAction->GetParentCanvas();

	// Clear the parent canvas and prepare for redraw
	UndoCanvas->Clear();

	// Redraw with left actions
	for (UDrawingActionBase* DrawingActionBase : DrawingActionStack)
	{
		if (DrawingActionBase->HasCanvasToDraw())
		{
			DrawingActionBase->ApplyToCanvas();
		}
	}

	// Destruct undo action
	UndoAction->ConditionalBeginDestroy();
}
