#include "DrawingSystem/DrawingAction_Pencil.h"

#include "Actors/DrawingCanvas.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"

void UDrawingAction_Pencil::ApplyToCanvas()
{
	Super::ApplyToCanvas();

	// Draw all points recorded
	DrawBrushAtPoints(DrawPoints);
}

void UDrawingAction_Pencil::AddInputPoint(FVector2D NewPoint)
{
	// Append this new point to existing line
	AppendDrawPoint(NewPoint);
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
