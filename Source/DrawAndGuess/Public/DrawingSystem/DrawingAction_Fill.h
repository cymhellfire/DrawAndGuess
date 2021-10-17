// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DrawingSystem/DrawingActionBase.h"
#include "DrawingAction_Fill.generated.h"

/**
 * 
 */
UCLASS()
class DRAWANDGUESS_API UDrawingAction_Fill : public UDrawingActionBase
{
	GENERATED_BODY()

public:
	UDrawingAction_Fill();

	virtual void StopInput(FVector2D StopPoint) override;
	virtual void ApplyToCanvas() override;
	virtual bool IsActionValid() override;

protected:

	void FillCanvas();
};
