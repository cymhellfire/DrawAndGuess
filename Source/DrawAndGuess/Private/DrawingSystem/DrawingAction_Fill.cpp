// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawingSystem/DrawingAction_Fill.h"

#include "Actors/DrawingCanvas.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"

UDrawingAction_Fill::UDrawingAction_Fill()
{
}

void UDrawingAction_Fill::StopInput(FVector2D StopPoint)
{
	if (StopPoint != NullVector && HasCanvasToDraw())
	{
		FillCanvas();
	}
}

void UDrawingAction_Fill::ApplyToCanvas()
{
	Super::ApplyToCanvas();

	FillCanvas();
}

bool UDrawingAction_Fill::IsActionValid()
{
	return HasCanvasToDraw();
}

void UDrawingAction_Fill::FillCanvas()
{
	// Initial drawing requirements
	UCanvas* DrawingCanvas;
	FVector2D DrawingCanvasSize;
	FDrawToRenderTargetContext NewContext;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), ParentCanvas->GetCanvasRenderTarget(), DrawingCanvas, DrawingCanvasSize, NewContext);

	DrawingCanvas->K2_DrawPolygon(nullptr, DrawingCanvasSize / 2, DrawingCanvasSize, 4, BrushSettings.BrushColor);

	// Apply canvas
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), NewContext);
}
