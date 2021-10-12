// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/DAGTestGameMode.h"

#include "EngineUtils.h"
#include "Actors/DrawingCanvas.h"
#include "Pawns/DrawingBrush.h"

void ADAGTestGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (ADrawingBrush* ControlledBrush = Cast<ADrawingBrush>(NewPlayer->GetPawn()))
	{
		for (TActorIterator<ADrawingCanvas> Iter(GetWorld()); Iter; ++Iter)
		{
			ControlledBrush->AddAcceptCanvas(*Iter);
		}
	}
}
