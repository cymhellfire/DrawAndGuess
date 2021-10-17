#include "DrawingSystem/DrawingActionManager.h"
#include "Actors/DrawingCanvas.h"
#include "DrawingSystem/DrawingActionBase.h"
#include "DrawingSystem/DrawingAction_Box.h"
//#include "DrawingSystem/DrawingAction_Circle.h"
#include "DrawingSystem/DrawingAction_Fill.h"
#include "DrawingSystem/DrawingAction_Line.h"
#include "DrawingSystem/DrawingAction_Pencil.h"
#include "Engine/ActorChannel.h"

ADrawingActionManager::ADrawingActionManager()
{
	bReplicates = true;
	bAlwaysRelevant = true;

	LastOperatedCanvas = nullptr;
}

UDrawingActionBase* ADrawingActionManager::CreateDrawingAction(EDrawingActionType ActionType)
{
	UDrawingActionBase* NewAction = nullptr;
	switch(ActionType)
	{
	case DAT_Pencil:
		NewAction = NewObject<UDrawingAction_Pencil>(this);
		break;
	case DAT_Line:
		NewAction = NewObject<UDrawingAction_Line>(this);
		break;
	case DAT_Box:
		NewAction = NewObject<UDrawingAction_Box>(this);
		break;
	case DAT_Circle:
		//NewAction = NewObject<UDrawingAction_Circle>(this);
		break;
	case DAT_Fill:
		NewAction = NewObject<UDrawingAction_Fill>(this);
		break;
	default: ;
	}

	return NewAction;
}

bool ADrawingActionManager::SubmitDrawingAction(UDrawingActionBase* DrawingAction)
{
	const bool Result = DrawingAction->IsActionValid();

	// Only valid action can be accepted
	if (Result)
	{
		ADrawingCanvas* TargetCanvas = DrawingAction->GetParentCanvas();
		if (DrawingActionMap.Contains(TargetCanvas))
		{
			DrawingActionMap[TargetCanvas].AddUnique(DrawingAction);
		}
		else
		{
			TArray<UDrawingActionBase*> NewActionStack;
			NewActionStack.Add(DrawingAction);
			DrawingActionMap.Add(TargetCanvas, NewActionStack);
		}

		// Register new action to target canvas
		TargetCanvas->RegisterDrawingAction(DrawingAction);

		// Record this canvas
		LastOperatedCanvas = TargetCanvas;
	}

	return Result;
}

void ADrawingActionManager::Undo()
{
	if (LastOperatedCanvas == nullptr)
	{
		UE_LOG(LogInit, Log, TEXT("[DrawingActionManager] No canvas selected."));
		return;
	}

	if (!DrawingActionMap.Contains(LastOperatedCanvas))
	{
		UE_LOG(LogInit, Log, TEXT("[DrawingActionManager] No drawing record on canvas %s"), *LastOperatedCanvas->GetName());
		return;
	}

	TArray<UDrawingActionBase*>* DrawingActionStack = DrawingActionMap.Find(LastOperatedCanvas);
	if (DrawingActionStack->Num() == 0)
	{
		UE_LOG(LogInit, Log, TEXT("[DrawingActionManager] Nothing to undo."));
		return;
	}

	// Pop the last action
	UDrawingActionBase* UndoAction = DrawingActionStack->Pop();
	ADrawingCanvas* UndoCanvas = UndoAction->GetParentCanvas();

	// Unregister from parent canvas
	UndoCanvas->UnregisterDrawingAction(UndoAction);
	UndoCanvas->Refresh();

	// Destruct undo action
	UndoAction->ConditionalBeginDestroy();
}

void ADrawingActionManager::Clear()
{
	if (LastOperatedCanvas == nullptr)
	{
		UE_LOG(LogInit, Log, TEXT("[DrawingActionManager] No canvas selected."));
		return;
	}

	if (!DrawingActionMap.Contains(LastOperatedCanvas))
	{
		UE_LOG(LogInit, Log, TEXT("[DrawingActionManager] No drawing record on canvas %s"), *LastOperatedCanvas->GetName());
		return;
	}

	TArray<UDrawingActionBase*>* DrawingActionStack = DrawingActionMap.Find(LastOperatedCanvas);
	if (DrawingActionStack->Num() == 0)
	{
		UE_LOG(LogInit, Log, TEXT("[DrawingActionManager] Nothing to clear."));
		return;
	}
	UDrawingActionBase* SampleAction = (*DrawingActionStack)[0];
	ADrawingCanvas* ClearCanvas = SampleAction->GetParentCanvas();

	// Clear all actions
	for (UDrawingActionBase* ActionBase : *DrawingActionStack)
	{
		ClearCanvas->UnregisterDrawingAction(ActionBase);
		ActionBase->ConditionalBeginDestroy();
	}
	DrawingActionStack->Empty();

	ClearCanvas->Refresh();
}
