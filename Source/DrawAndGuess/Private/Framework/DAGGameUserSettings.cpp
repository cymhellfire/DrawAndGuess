// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/DAGGameUserSettings.h"

UDAGGameUserSettings::UDAGGameUserSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PencilInterpolationThreshold = 0.1f;
	PlayerName = TEXT("NewPlayer");
}

void UDAGGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();

	PencilInterpolationThreshold = 0.1f;
	PlayerName = TEXT("NewPlayer");
}

float UDAGGameUserSettings::GetPencilInterpolateThreshold() const
{
	return PencilInterpolationThreshold;
}

FString UDAGGameUserSettings::GetPlayerName() const
{
	return PlayerName;
}
