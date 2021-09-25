#include "DrawingSystem/DrawingActionManager.h"

#include "Actors/DrawingCanvas.h"
#include "DrawingSystem/DrawingActionBase.h"
#include "DrawingSystem/DrawingAction_Pencil.h"

UDrawingActionManager* UDrawingActionManager::Instance = nullptr;

UDrawingActionManager* UDrawingActionManager::GetInstance()
{
	// Create a new instance if no available one
	if (Instance == nullptr)
	{
		//Instance = NewObject<UDrawingActionManager>(GEngine->GetWorld(), UDrawingActionManager::StaticClass());
		Instance = NewObject<UDrawingActionManager>(GEngine->GetCurrentPlayWorld());
		Instance->Initialize();
	}

	return Instance;
}

void UDrawingActionManager::Initialize()
{
	// Listen to world clean up event for self-destruction
	WorldCleanupDelegateHandle = FWorldDelegates::OnWorldCleanup.AddUObject(this, &UDrawingActionManager::OnWorldCleanup);
}

void UDrawingActionManager::OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources)
{
	FWorldDelegates::OnWorldCleanup.Remove(WorldCleanupDelegateHandle);

	// Clear the reference and destroy myself
	UDrawingActionManager::Instance = nullptr;
	MarkPendingKill();
}

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
		DrawingActionBase->ApplyToCanvas();
	}

	// Destruct undo action
	UndoAction->ConditionalBeginDestroy();
}
