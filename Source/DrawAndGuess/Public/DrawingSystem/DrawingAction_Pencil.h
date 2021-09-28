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
	UDrawingAction_Pencil();

	virtual void ApplyToCanvas() override;

	virtual void AddInputPoint(FVector2D NewPoint) override;

	virtual void CopyBrushSettings(ADrawingBrush* Brush) override;

protected:
	/**
	 * Interpolate extra points between origin and destination.
	 *
	 * @param Origin			The origin point
	 * @param Dest				The destination point
	 * @param Threshold			Threshold to determine if there should be a interpolated point
	 * @return An array of interpolated points
	 */
	TArray<FVector2D> InterpolateDrawingPoints(FVector2D Origin, FVector2D Dest, float Threshold);

	/**
	* Append a new point to pencil line.
	*
	* @param NewPoint			New point to draw at.
	*/
	void AppendDrawPoint(FVector2D NewPoint);

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
	TArray<FVector2D> DrawPoints;

	FVector2D LastPoint = FVector2D(-1, -1);

	float InterpolationThreshold;
};
