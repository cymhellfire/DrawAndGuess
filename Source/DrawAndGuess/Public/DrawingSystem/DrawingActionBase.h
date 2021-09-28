#pragma once

#include "CoreMinimal.h"
#include "DrawingSystemCommon.h"
#include "DrawingActionBase.generated.h"

class ADrawingCanvas;
class ADrawingBrush;

UCLASS()
class DRAWANDGUESS_API UDrawingActionBase : public UObject
{
	GENERATED_BODY()
public:

	void SetParentCanvas(ADrawingCanvas* NewCanvas);

	FORCEINLINE bool HasCanvasToDraw() const { return ParentCanvas != nullptr; }

	FORCEINLINE ADrawingCanvas* GetParentCanvas() const { return ParentCanvas; }

	/**
	 * Add a new input point to this action and invoke corresponding internal function.
	 *
	 * @param NewPoint		New input point to react with.
	 */
	virtual void AddInputPoint(FVector2D NewPoint) {}

	/**
	 * Copy necessary settings from given drawing brush.
	 *
	 * @param Brush			The brush copy settings from.
	 */
	virtual void CopyBrushSettings(ADrawingBrush* Brush);

	/**
	 * Apply this drawing action to parent DrawingCanvas.
	 */
	virtual void ApplyToCanvas();

protected:
	ADrawingCanvas* ParentCanvas;

	FDrawingBrushSettings BrushSettings;
};
