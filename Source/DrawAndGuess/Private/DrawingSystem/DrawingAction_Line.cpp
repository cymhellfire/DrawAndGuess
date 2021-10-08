// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawingSystem/DrawingAction_Line.h"

#include "Actors/DrawingCanvas.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"

UDrawingAction_Line::UDrawingAction_Line()
{
	Origin = Destination = PreviewPoint = NullVector;
}

void UDrawingAction_Line::AddInputPoint(FVector2D NewPoint)
{
	// Record origin
	if (Origin == NullVector)
	{
		Origin = NewPoint;
	}
}

void UDrawingAction_Line::UpdatePreviewPoint(FVector2D NewPoint)
{
	PreviewPoint = NewPoint;

	if (Origin != NullVector)
	{
		DrawPreviewLine();
	}
}

void UDrawingAction_Line::StopInput(FVector2D StopPoint)
{
	// Only draw the line if origin and destination are valid
	if (Origin != NullVector && StopPoint != NullVector)
	{
		// Record the destination
		Destination = StopPoint;

		DrawLine();
	}

	// Clear last preview
	if (ParentCanvas)
	{
		ParentCanvas->ClearPreview();
	}
}

void UDrawingAction_Line::ApplyToCanvas()
{
	Super::ApplyToCanvas();

	DrawLine();
}

bool UDrawingAction_Line::IsActionValid()
{
	return ParentCanvas && (Origin != NullVector) && (Destination != NullVector); 
}

void UDrawingAction_Line::DrawLine()
{
	// Initial drawing requirements
	UCanvas* DrawingCanvas;
	FVector2D DrawingCanvasSize;
	FDrawToRenderTargetContext NewContext;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), ParentCanvas->GetCanvasRenderTarget(), DrawingCanvas, DrawingCanvasSize, NewContext);

	FVector2D OrgInCanvas = DrawingCanvasSize * Origin;
	FVector2D DestInCanvas = DrawingCanvasSize * Destination;

	// Draw texture
	DrawingCanvas->K2_DrawLine(OrgInCanvas, DestInCanvas, BrushSettings.BrushSize, BrushSettings.BrushColor);

	// Apply canvas
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), NewContext);
}

void UDrawingAction_Line::DrawPreviewLine()
{
	// Clear last preview
	ParentCanvas->ClearPreview();

	// Initial drawing requirements
	UCanvas* DrawingCanvas;
	FVector2D DrawingCanvasSize;
	FDrawToRenderTargetContext NewContext;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), ParentCanvas->GetCanvasPreviewRenderTarget(), DrawingCanvas, DrawingCanvasSize, NewContext);

	FVector2D OrgInCanvas = DrawingCanvasSize * Origin;
	FVector2D DestInCanvas = DrawingCanvasSize * PreviewPoint;

	// Draw texture
	DrawingCanvas->K2_DrawLine(OrgInCanvas, DestInCanvas, BrushSettings.BrushSize, BrushSettings.BrushColor);

	// Apply canvas
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), NewContext);
}
