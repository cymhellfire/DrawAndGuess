#include "DrawingSystem/DrawingActionManager.h"
#include "Net/UnrealNetwork.h"
#include "Actors/DrawingCanvas.h"
#include "DrawingSystem/DrawingActionBase.h"
#include "DrawingSystem/DrawingAction_Box.h"
#include "DrawingSystem/DrawingAction_Line.h"
#include "DrawingSystem/DrawingAction_Pencil.h"
#include "Engine/ActorChannel.h"
#include "Net/Core/PushModel/PushModel.h"

ADrawingActionManager::ADrawingActionManager()
{
	bReplicates = true;
	bAlwaysRelevant = true;
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
		DrawingActionStack.AddUnique(DrawingAction);

		MARK_PROPERTY_DIRTY_FROM_NAME(ADrawingActionManager, ActionCount, this);
		ActionCount = DrawingActionStack.Num();
	}

	return Result;
}

void ADrawingActionManager::Undo()
{
	if (DrawingActionStack.Num() == 0)
	{
		UE_LOG(LogInit, Log, TEXT("[DrawingActionManager] Nothing to undo."));
		return;
	}

	// Pop the last action
	UDrawingActionBase* UndoAction = DrawingActionStack.Pop();
	ADrawingCanvas* UndoCanvas = UndoAction->GetParentCanvas();

	MARK_PROPERTY_DIRTY_FROM_NAME(ADrawingActionManager, ActionCount, this);
	ActionCount = DrawingActionStack.Num();

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

void ADrawingActionManager::OnRep_ActionCount()
{
	UE_LOG(LogInit, Log, TEXT("[Manager] Action Count to %d"), ActionCount);
}

void ADrawingActionManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS(ADrawingActionManager, ActionCount, SharedParams);
}
