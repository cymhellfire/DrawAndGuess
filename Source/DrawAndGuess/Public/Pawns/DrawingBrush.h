// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DrawingBrush.generated.h"

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
	 * Draw a texture to render target with current brush setting.
	 * 
	 * @param RenderTarget		Render target to draw on.
	 * @param Location			UV coordinate [0.0 - 1.0] to draw texture on render target.
	 */
	void DrawBrush(UTextureRenderTarget2D* RenderTarget, FVector2D Location);

	void OnDrawButtonPressed();
	void OnDrawButtonReleased();

public:
	virtual void PossessedBy(AController* NewController) override;

	virtual void UnPossessed() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetBrushSize(float NewSize) { BrushSize = NewSize; }

	void SetBrushTexture(UTexture2D* NewTexture);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DrawingBrush")
	UMaterial* BrushMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DrawingBrush")
	UTexture2D* BrushTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DrawingBrush")
	float BrushSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DrawingBrush")
	FLinearColor BrushColor;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* BrushMaterialInstance;

	bool bDrawing;
};
