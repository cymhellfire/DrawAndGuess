#include "DrawingSystem/DrawingActionBase.h"

#include "Actors/DrawingCanvas.h"

void UDrawingActionBase::SetParentCanvas(ADrawingCanvas* NewCanvas)
{
	ParentCanvas = NewCanvas;
}

void UDrawingActionBase::ApplyToCanvas()
{
	checkf(IsValid(ParentCanvas), TEXT("[UDrawingActionBase] No valid parent canvas to apply to."));
}
