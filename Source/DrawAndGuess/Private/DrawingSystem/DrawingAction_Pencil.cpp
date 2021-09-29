#include "DrawingSystem/DrawingAction_Pencil.h"

#include "Actors/DrawingCanvas.h"
#include "Engine/Canvas.h"
#include "Framework/DAGGameUserSettings.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Pawns/DrawingBrush.h"

UDrawingAction_Pencil::UDrawingAction_Pencil()
{
	// Get the pencil interpolation settings
	if (GEngine != nullptr)
	{
		if (UDAGGameUserSettings* GameUserSettings = Cast<UDAGGameUserSettings>(GEngine->GetGameUserSettings()))
		{
			InterpolationThreshold = GameUserSettings->GetPencilInterpolateThreshold();
		}
	}

	LastPoint = NullVector;
}

void UDrawingAction_Pencil::ApplyToCanvas()
{
	Super::ApplyToCanvas();

	// Draw all points recorded
	DrawBrushAtPoints(DrawPoints);
}

void UDrawingAction_Pencil::UpdatePreviewPoint(FVector2D NewPoint)
{
	// Pencil will directly append new point into existing line
	AddInputPoint(NewPoint);
}

void UDrawingAction_Pencil::AddInputPoint(FVector2D NewPoint)
{
	// Append this new point to existing line
	if (InterpolationThreshold != 0.f && LastPoint != NullVector)
	{
		TArray<FVector2D> InterpolatePoints = InterpolateDrawingPoints(LastPoint, NewPoint, InterpolationThreshold);
		if (InterpolatePoints.Num() > 0)
		{
			for (FVector2D Interpoint : InterpolatePoints)
			{
				AppendDrawPoint(Interpoint);
			}
		}
		AppendDrawPoint(NewPoint);
	}
	else
	{
		AppendDrawPoint(NewPoint);
	}

	LastPoint = NewPoint;
}

void UDrawingAction_Pencil::CopyBrushSettings(ADrawingBrush* Brush)
{
	Super::CopyBrushSettings(Brush);
}

bool UDrawingAction_Pencil::IsActionValid()
{
	return ParentCanvas && DrawPoints.Num() > 0;
}

TArray<FVector2D> UDrawingAction_Pencil::InterpolateDrawingPoints(FVector2D Origin, FVector2D Dest, float Threshold)
{
	TArray<FVector2D> Result;

	// We need parent canvas for getting correct canvas size
	if (HasCanvasToDraw())
	{
		float BrushSize = BrushSettings.BrushSize;
		const FVector2D CanvasSize = ParentCanvas->GetCanvasSize();
		FVector2D DeltaPos = (Dest - Origin);
		FVector2D DeltaPosInCanvas = DeltaPos * CanvasSize;
		float DeltaDist = DeltaPosInCanvas.Size();
		int32 InterpolateCount = FMath::FloorToInt(DeltaDist / BrushSize / Threshold);
		for (int32 Index = 0; Index < InterpolateCount; ++Index)
		{
			Result.Add(Origin + DeltaPos * (Index + 1) / InterpolateCount);
		}
	}

	return Result;
}

void UDrawingAction_Pencil::ApplyToCanvasIncremental(FVector2D NewPoint)
{
	DrawBrushAtPoint(NewPoint);
}

void UDrawingAction_Pencil::DrawBrushAtPoint(FVector2D Location)
{
	// Initial drawing requirements
	UCanvas* DrawingCanvas;
	FVector2D DrawingCanvasSize;
	FDrawToRenderTargetContext NewContext;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), ParentCanvas->GetCanvasRenderTarget(), DrawingCanvas, DrawingCanvasSize, NewContext);

	// Calculate draw location
	FVector2D DrawLocation = DrawingCanvasSize * Location - BrushSettings.BrushSize / 2;
	FVector2D DrawBrushSize(BrushSettings.BrushSize);

	// Apply brush settings
	ApplyBrushSettingsToMaterial(ParentCanvas->GetDrawingBrushMaterial());

	// Draw texture
	DrawingCanvas->K2_DrawMaterial(ParentCanvas->GetDrawingBrushMaterial(), DrawLocation, DrawBrushSize, FVector2D::ZeroVector);

	// Apply canvas
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), NewContext);
}

void UDrawingAction_Pencil::DrawBrushAtPoints(TArray<FVector2D>& LocationList)
{
	// Initial drawing requirements
	UCanvas* DrawingCanvas;
	FVector2D DrawingCanvasSize;
	FDrawToRenderTargetContext NewContext;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), ParentCanvas->GetCanvasRenderTarget(), DrawingCanvas, DrawingCanvasSize, NewContext);

	// Apply brush settings
	ApplyBrushSettingsToMaterial(ParentCanvas->GetDrawingBrushMaterial());

	// Draw at every point in list
	for (FVector2D CurrentLocation : LocationList)
	{
		// Calculate draw location
		FVector2D DrawLocation = DrawingCanvasSize * CurrentLocation - BrushSettings.BrushSize / 2;
		FVector2D DrawBrushSize(BrushSettings.BrushSize);

		// Draw texture
		DrawingCanvas->K2_DrawMaterial(ParentCanvas->GetDrawingBrushMaterial(), DrawLocation, DrawBrushSize, FVector2D::ZeroVector);
	}

	// Apply canvas
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), NewContext);
}

void UDrawingAction_Pencil::AppendDrawPoint(FVector2D NewPoint)
{
	DrawPoints.Add(NewPoint);

	// Immediately draw the new point to canvas
	ApplyToCanvasIncremental(NewPoint);
}

void UDrawingAction_Pencil::ApplyBrushSettingsToMaterial(UMaterialInstanceDynamic* TargetMaterial)
{
	// Change texture
	TargetMaterial->SetTextureParameterValue(TEXT("BrushTexture"), BrushSettings.BrushTexture);

	// Change brush color
	TargetMaterial->SetVectorParameterValue(TEXT("BrushColor"), BrushSettings.BrushColor);
}
