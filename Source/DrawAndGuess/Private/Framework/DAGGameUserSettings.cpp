// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/DAGGameUserSettings.h"

UDAGGameUserSettings::UDAGGameUserSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PencilInterpolationThreshold = 0.1f;
	PlayerName = TEXT("NewPlayer");
	DrawingTimePerRound = 60;
	MaxDrawingRound = 3;
	CandidateWordCount = 3;
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

void UDAGGameUserSettings::SetPlayerName(const FString& NewName)
{
	PlayerName = NewName;
}

FString UDAGGameUserSettings::GetPlayerName() const
{
	return PlayerName;
}

void UDAGGameUserSettings::SetLastRoomIP(const FString& NewIp)
{
	LastRoomIP = NewIp;
}

void UDAGGameUserSettings::SetWordPoolFilePath(const FString& NewPath)
{
	WordPoolFilePath = NewPath;
}

FString UDAGGameUserSettings::GetWordPoolFileFolder() const
{
	if (!WordPoolFilePath.IsEmpty())
	{
		int32 CharIndex = -1;
		if (WordPoolFilePath.FindLastChar('/', CharIndex))
		{
			return WordPoolFilePath.Mid(0, CharIndex);
		}
	}

	return "";
}

void UDAGGameUserSettings::SetDrawingTimePerRound(int32 NewTime)
{
	DrawingTimePerRound = NewTime;
}

void UDAGGameUserSettings::SetCandidateWordCount(int32 NewCount)
{
	CandidateWordCount = NewCount;
}

void UDAGGameUserSettings::SetMaxDrawingRound(int32 NewCount)
{
	MaxDrawingRound = NewCount;
}

UDAGGameUserSettings* UDAGGameUserSettings::GetDAGGameUserSettings()
{
	if (GEngine)
	{
		return Cast<UDAGGameUserSettings>(GEngine->GetGameUserSettings());
	}

	return nullptr;
}
