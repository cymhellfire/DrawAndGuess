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
	 * Update the preview point location to refresh this action preview.
	 *
	 * @param NewPoint	New point to preview the action result.
	 */
	virtual void UpdatePreviewPoint(FVector2D NewPoint) {}

	/**
	 * Tell this action that no input will be received.
	 *
	 * @param StopPoint		The point when input stop.
	 */
	virtual void StopInput(FVector2D StopPoint) {}

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

	/**
	 * Check if this drawing action is valid to draw.
	 *
	 * @return Validation result.
	 */
	virtual bool IsActionValid() { return false; }

protected:
	ADrawingCanvas* ParentCanvas;

	FDrawingBrushSettings BrushSettings;
};
