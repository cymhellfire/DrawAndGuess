#pragma once

#include "CoreMinimal.h"
#include "DrawingActionBase.h"
#include "DrawingSystemCommon.h"
#include "DrawingAction_Pencil.generated.h"

UCLASS()
class DRAWANDGUESS_API UDrawingAction_Pencil : public UDrawingActionBase
{
	GENERATED_BODY()
public:

	virtual void ApplyToCanvas() override;

	/**
	 * Append a new point to pencil line.
	 *
	 * @param NewPoint			New point to draw at.
	 */
	void AppendDrawPoint(FVector2D NewPoint);

	void CopyBrushSetting(class ADrawingBrush* Brush);

protected:

	/**
	 * Incrementally apply new points to canvas.
	 *
	 * @param NewPoint			New point to draw at.
	 */
	void ApplyToCanvasIncremental(FVector2D NewPoint);

	/**
	 * Draw at a point with current brush setting.
	 *
	 * @param Location			Target point to draw at.
	 */
	void DrawBrushAtPoint(FVector2D Location);

	/**
	* Draw at a list of point with current brush setting.
	*
	* @param LocationList			List of target points to draw at.
	*/
	void DrawBrushAtPoints(TArray<FVector2D>& LocationList);

	void ApplyBrushSettingsToMaterial(UMaterialInstanceDynamic* TargetMaterial);

protected:
	FDrawingBrushSettings BrushSettings;

	TArray<FVector2D> DrawPoints;
};
