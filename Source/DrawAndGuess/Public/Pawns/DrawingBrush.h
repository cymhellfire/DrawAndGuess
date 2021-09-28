// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DrawingSystem/DrawingSystemCommon.h"
#include "GameFramework/Pawn.h"
#include "DrawingBrush.generated.h"

class UDrawingActionBase;

UCLASS()
class DRAWANDGUESS_API ADrawingBrush : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADrawingBrush();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnDrawButtonPressed();
	void OnDrawButtonReleased();

	void OnUndoPressed();

	void OnCursorAxisChanged(float Input);

public:
	virtual void PossessedBy(AController* NewController) override;

	virtual void UnPossessed() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category="DrawingBrush")
	void SetBrushSize(float NewSize) { BrushSize = NewSize; }

	float GetBrushSize() const { return BrushSize; }

	void SetBrushTexture(UTexture2D* NewTexture);

	UTexture2D* GetBrushTexture() const { return BrushTexture; }

	UFUNCTION(BlueprintCallable, Category="DrawingBrush")
	void SetBrushColor(FLinearColor NewColor);

	FLinearColor GetBrushColor() const { return BrushColor; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DrawingBrush")
	UMaterial* BrushMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DrawingBrush")
	UTexture2D* BrushTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DrawingBrush")
	float BrushSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DrawingBrush")
	FLinearColor BrushColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DrawingBrush")
	TEnumAsByte<EDrawingActionType> DrawingActionType;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* BrushMaterialInstance;

	bool bDrawing;
	bool bPendingDrawing;

	UDrawingActionBase* CurrentDrawAction;
};
