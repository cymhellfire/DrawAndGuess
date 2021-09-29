// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawingSystem/DrawingAction_Line.h"

#include "Actors/DrawingCanvas.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"

UDrawingAction_Line::UDrawingAction_Line()
{
	Origin = Destination = NullVector;
}

void UDrawingAction_Line::AddInputPoint(FVector2D NewPoint)
{
	// Record origin first
	if (Origin == NullVector)
	{
		Origin = NewPoint;
	}
	else if (Destination == NullVector)
	{
		Destination = NewPoint;
	}
}

void UDrawingAction_Line::StopInput(FVector2D StopPoint)
{
	// Record the destination
	Destination = StopPoint;

	DrawLine();
}

void UDrawingAction_Line::ApplyToCanvas()
{
	Super::ApplyToCanvas();

	DrawLine();
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