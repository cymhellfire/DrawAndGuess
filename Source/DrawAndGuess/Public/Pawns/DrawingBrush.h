// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DrawingSystem/DrawingSystemCommon.h"
#include "GameFramework/Pawn.h"
#include "DrawingBrush.generated.h"

class UDrawingActionBase;
class ADrawingCanvas;

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

	/**
	 * Get the drawing point location in given canvas.
	 * Or get the drawing point location and drawing canvas.
	 *
	 * @param DesiredCanvas			Given a canvas that drawing point belong to.
	 * (Pass in nullptr will be modified to detected canvas.)
	 * @return	The UV coordinate of drawing point in canvas space.
	 * (NullVector when no drawing point match requirement.)
	 */
	FVector2D GetDrawingPoint(ADrawingCanvas*& DesiredCanvas);

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
	bool bPendingStopInput;

	UDrawingActionBase* CurrentDrawAction;
};
