// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawingSystem/DrawingAction_Box.h"
#include "Actors/DrawingCanvas.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"

UDrawingAction_Box::UDrawingAction_Box()
{
	Origin = Destination = PreviewPoint = NullVector;
}

void UDrawingAction_Box::AddInputPoint(FVector2D NewPoint)
{
	if (Origin == NullVector)
	{
		Origin = NewPoint;
	}
}

void UDrawingAction_Box::UpdatePreviewPoint(FVector2D NewPoint)
{
	PreviewPoint = NewPoint;

	if (PreviewPoint != NullVector)
	{
		DrawPreviewBox();
	}
}

void UDrawingAction_Box::StopInput(FVector2D StopPoint)
{
	if (Origin != NullVector && StopPoint != NullVector)
	{
		Destination = StopPoint;

		DrawBox();
	}

	// Clear last preview
	if (ParentCanvas)
	{
		ParentCanvas->ClearPreview();
	}
}

void UDrawingAction_Box::ApplyToCanvas()
{
	Super::ApplyToCanvas();

	DrawBox();
}

bool UDrawingAction_Box::IsActionValid()
{
	return ParentCanvas && (Origin != NullVector) && (Destination != NullVector);
}

void UDrawingAction_Box::DrawPreviewBox()
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

	FVector2D DrawingPos = FVector2D(FMath::Min(OrgInCanvas.X, DestInCanvas.X), FMath::Min(OrgInCanvas.Y, DestInCanvas.Y));
	FVector2D DrawingSize = FVector2D(FMath::Abs(OrgInCanvas.X - DestInCanvas.X), FMath::Abs(OrgInCanvas.Y - DestInCanvas.Y));

	// Draw box
	DrawingCanvas->K2_DrawBox(DrawingPos, DrawingSize, BrushSettings.BrushSize, BrushSettings.BrushColor);

	// Apply canvas
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), NewContext);
}

void UDrawingAction_Box::DrawBox()
{
	// Initial drawing requirements
	UCanvas* DrawingCanvas;
	FVector2D DrawingCanvasSize;
	FDrawToRenderTargetContext NewContext;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), ParentCanvas->GetCanvasRenderTarget(), DrawingCanvas, DrawingCanvasSize, NewContext);

	FVector2D OrgInCanvas = DrawingCanvasSize * Origin;
	FVector2D DestInCanvas = DrawingCanvasSize * Destination;
	
	FVector2D DrawingPos = FVector2D(FMath::Min(OrgInCanvas.X, DestInCanvas.X), FMath::Min(OrgInCanvas.Y, DestInCanvas.Y));
	FVector2D DrawingSize = FVector2D(FMath::Abs(OrgInCanvas.X - DestInCanvas.X), FMath::Abs(OrgInCanvas.Y - DestInCanvas.Y));

	// Draw box
	DrawingCanvas->K2_DrawBox(DrawingPos, DrawingSize, BrushSettings.BrushSize, BrushSettings.BrushColor);

	// Apply canvas
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), NewContext);
}
