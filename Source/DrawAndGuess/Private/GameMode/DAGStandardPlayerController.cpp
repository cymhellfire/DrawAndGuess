// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/DAGStandardPlayerController.h"

#include "GameMode/DAGStandardGameMode.h"

void ADAGStandardPlayerController::ServerChooseWord_Implementation(int32 Index)
{
	if (ADAGStandardGameMode* StandardGameMode = Cast<ADAGStandardGameMode>(GetWorld()->GetAuthGameMode()))
	{
		StandardGameMode->ChooseWordByIndex(Index);
	}
}
