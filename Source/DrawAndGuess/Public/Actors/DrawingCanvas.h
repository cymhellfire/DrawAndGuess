// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Actor.h"
#include "DrawingCanvas.generated.h"

class UDrawingActionBase;

USTRUCT(BlueprintType)
struct FCanvasSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Width;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Height;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<ETextureRenderTargetFormat> RenderTargetFormat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor ClearColor;

	FCanvasSettings()
	{
		Width = 1024;
		Height = 1024;
		RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
		ClearColor = FLinearColor::White;
	}
};

UCLASS()
class DRAWANDGUESS_API ADrawingCanvas : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADrawingCanvas();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category="DrawingCanvas")
	UTextureRenderTarget2D* GetCanvasRenderTarget() const { return CanvasRenderTarget; }

	UTextureRenderTarget2D* GetCanvasPreviewRenderTarget() const { return CanvasPreviewRenderTarget; }

	/* Get the actual size of canvas render target. */
	FVector2D GetCanvasSize() const { return FVector2D(CanvasSetting.Width, CanvasSetting.Height); }

	/**
	 * Get the public brush material instance provided for drawing actions.
	 * 
	 * @return		The brush material for drawing actions.
	 */
	UMaterialInstanceDynamic* GetDrawingBrushMaterial() { return BrushMaterialInstance; }

	/**
	 * Reset canvas to blank state.
	 */
	void Clear();

	/**
	 * Clear the preview canvas.
	 */
	void ClearPreview();

	void RegisterDrawingAction(UDrawingActionBase* NewDrawingAction);

	void UnregisterDrawingAction(UDrawingActionBase* ActionToRemove);

	/**
	 * Clear and redraw this canvas.
	 */
	void Refresh();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Canvas")
	UStaticMeshComponent* CanvasMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Canvas")
	UMaterial* CanvasMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Canvas")
	UMaterial* BrushMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Canvas")
	FCanvasSettings CanvasSetting;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* CanvasMaterialInstance;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* BrushMaterialInstance;

	UPROPERTY(Transient)
	UTextureRenderTarget2D* CanvasRenderTarget;

	UPROPERTY(Transient)
	UTextureRenderTarget2D* CanvasPreviewRenderTarget;

	UPROPERTY(Transient)
	TArray<UDrawingActionBase*> DrawingActionStack;
};
