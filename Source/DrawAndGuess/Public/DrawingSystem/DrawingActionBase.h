#pragma once

#include "CoreMinimal.h"
#include "DrawingActionBase.generated.h"

class ADrawingCanvas;

UCLASS()
class DRAWANDGUESS_API UDrawingActionBase : public UObject
{
	GENERATED_BODY()
public:

	void SetParentCanvas(ADrawingCanvas* NewCanvas);

	ADrawingCanvas* GetParentCanvas() const { return ParentCanvas; }

	/**
	 * Apply this drawing action to parent DrawingCanvas.
	 */
	virtual void ApplyToCanvas();

protected:
	ADrawingCanvas* ParentCanvas;
};
