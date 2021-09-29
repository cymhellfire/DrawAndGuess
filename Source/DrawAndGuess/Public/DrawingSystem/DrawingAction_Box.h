// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DrawingSystem/DrawingActionBase.h"
#include "DrawingAction_Box.generated.h"

/**
 * 
 */
UCLASS()
class DRAWANDGUESS_API UDrawingAction_Box : public UDrawingActionBase
{
	GENERATED_BODY()

public:
	UDrawingAction_Box();

	virtual void AddInputPoint(FVector2D NewPoint) override;
	virtual void UpdatePreviewPoint(FVector2D NewPoint) override;
	virtual void StopInput(FVector2D StopPoint) override;
	virtual void ApplyToCanvas() override;
	virtual bool IsActionValid() override;

protected:

	void DrawPreviewBox();

	void DrawBox();

protected:
	FVector2D Origin;
	FVector2D Destination;
	FVector2D PreviewPoint;
};
