#include "DrawingSystem/DrawingActionBase.h"

#include "Actors/DrawingCanvas.h"
#include "Pawns/DrawingBrush.h"

void UDrawingActionBase::SetParentCanvas(ADrawingCanvas* NewCanvas)
{
	ParentCanvas = NewCanvas;
}

void UDrawingActionBase::CopyBrushSettings(ADrawingBrush* Brush)
{
	BrushSettings = FDrawingBrushSettings{
		Brush->GetBrushTexture(),
		Brush->GetBrushSize(),
		Brush->GetBrushColor()
	};
}

void UDrawingActionBase::ApplyToCanvas()
{
	checkf(IsValid(ParentCanvas), TEXT("[UDrawingActionBase] No valid parent canvas to apply to."));
}
