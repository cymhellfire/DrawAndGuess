// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DrawingSystem/DrawingActionBase.h"
#include "DrawingAction_Line.generated.h"

/**
 * 
 */
UCLASS()
class DRAWANDGUESS_API UDrawingAction_Line : public UDrawingActionBase
{
	GENERATED_BODY()

public:
	UDrawingAction_Line();

	virtual void AddInputPoint(FVector2D NewPoint) override;

	virtual void UpdatePreviewPoint(FVector2D NewPoint) override;

	virtual void StopInput(FVector2D StopPoint) override;

	virtual void ApplyToCanvas() override;

	virtual bool IsActionValid() override;

protected:

	void DrawLine();

	void DrawPreviewLine();

protected:

	FVector2D Origin;
	FVector2D Destination;
	FVector2D PreviewPoint;
};
