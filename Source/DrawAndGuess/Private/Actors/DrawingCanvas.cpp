// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/DrawingCanvas.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"

// Sets default values
ADrawingCanvas::ADrawingCanvas()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bAlwaysRelevant = true;

	// Construct the canvas mesh component
	CanvasMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("CanvasMesh");
	const ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane"));
	const ConstructorHelpers::FObjectFinder<UMaterial> PlaneMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
	CanvasMeshComponent->SetStaticMesh(PlaneMesh.Object);
	CanvasMeshComponent->SetMaterial(0, PlaneMaterial.Object);
	RootComponent = CanvasMeshComponent;
}

// Called when the game starts or when spawned
void ADrawingCanvas::BeginPlay()
{
	Super::BeginPlay();

	// Create the material dynamic instance
	if (IsValid(CanvasMaterial))
	{
		UObject* MIDOuter = GetWorld();
		CanvasMaterialInstance = UMaterialInstanceDynamic::Create(CanvasMaterial, MIDOuter);
		if (MIDOuter == nullptr)
		{
			CanvasMaterialInstance->SetFlags(RF_Transient);
		}
	}

	// Apply the material instance
	if (IsValid(CanvasMaterialInstance))
	{
		CanvasMeshComponent->SetMaterial(0, CanvasMaterialInstance);

		// Create the render target
		CanvasRenderTarget = NewObject<UTextureRenderTarget2D>(GetWorld());
		check(CanvasRenderTarget);
		CanvasRenderTarget->RenderTargetFormat = CanvasSetting.RenderTargetFormat;
		CanvasRenderTarget->ClearColor = CanvasSetting.ClearColor;
		CanvasRenderTarget->bAutoGenerateMips = false;
		CanvasRenderTarget->InitAutoFormat(CanvasSetting.Width, CanvasSetting.Height);
		CanvasRenderTarget->UpdateResourceImmediate(true);

		// Create preview render target
		CanvasPreviewRenderTarget = NewObject<UTextureRenderTarget2D>(GetWorld());
		check(CanvasPreviewRenderTarget);
		CanvasPreviewRenderTarget->RenderTargetFormat = CanvasSetting.RenderTargetFormat;
		CanvasPreviewRenderTarget->ClearColor = FLinearColor::Transparent;
		CanvasPreviewRenderTarget->bAutoGenerateMips = false;
		CanvasPreviewRenderTarget->InitAutoFormat(CanvasSetting.Width, CanvasSetting.Height);
		CanvasPreviewRenderTarget->UpdateResourceImmediate(true);

		// Setup render target
		CanvasMaterialInstance->SetTextureParameterValue(TEXT("RenderTarget"), CanvasRenderTarget);
		CanvasMaterialInstance->SetTextureParameterValue(TEXT("PreviewRenderTarget"), CanvasPreviewRenderTarget);
	}

	// Create public brush material instance
	if (IsValid(BrushMaterial))
	{
		UObject* MIDOuter = GetWorld();
		BrushMaterialInstance = UMaterialInstanceDynamic::Create(BrushMaterial, MIDOuter);
		if (MIDOuter == nullptr)
		{
			BrushMaterialInstance->SetFlags(RF_Transient);
		}
	}
}

// Called every frame
void ADrawingCanvas::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADrawingCanvas::Clear()
{
	UKismetRenderingLibrary::ClearRenderTarget2D(this, CanvasRenderTarget, CanvasSetting.ClearColor);
}

void ADrawingCanvas::ClearPreview()
{
	UKismetRenderingLibrary::ClearRenderTarget2D(this, CanvasPreviewRenderTarget, FLinearColor::Transparent);
}

