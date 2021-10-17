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

#if WITH_EDITOR
	ScreenshotSavePath = FString::Printf(TEXT("%s/Screenshots"), *FPaths::ProjectSavedDir());
#else
	ScreenshotSavePath = FString::Printf(TEXT("%s/Screenshots"), *FPaths::LaunchDir());
#endif
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

FString UDAGGameUserSettings::GetWordPoolFileName() const
{
	if (!WordPoolFilePath.IsEmpty())
	{
		int32 CharIndex = -1;
		if (WordPoolFilePath.FindLastChar('/', CharIndex))
		{
			return WordPoolFilePath.Mid(CharIndex + 1, WordPoolFilePath.Len() - CharIndex - 1);
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

void UDAGGameUserSettings::SetScreenshotSavePath(FString NewPath)
{
	ScreenshotSavePath = NewPath;
}

FString UDAGGameUserSettings::GetScreenshotSavePath() const
{
	if (!ScreenshotSavePath.IsEmpty())
	{
		// Check and create dictionary
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		if (!PlatformFile.DirectoryExists(*ScreenshotSavePath))
		{
			UE_LOG(LogInit, Log, TEXT("[GameUserSettings] Create screenshot dictionary %s"), *ScreenshotSavePath);

			PlatformFile.CreateDirectory(*ScreenshotSavePath);
		}

		return ScreenshotSavePath;
	}

	return "";
}

void UDAGGameUserSettings::PostInitProperties()
{
	Super::PostInitProperties();

	// Validate the screenshot path
	if (ScreenshotSavePath.IsEmpty())
	{
#if WITH_EDITOR
		ScreenshotSavePath = FString::Printf(TEXT("%s/Screenshots"), *FPaths::ProjectSavedDir());
#else
		ScreenshotSavePath = FString::Printf(TEXT("%s/Screenshots"), *FPaths::LaunchDir());
#endif
	}
}

UDAGGameUserSettings* UDAGGameUserSettings::GetDAGGameUserSettings()
{
	if (GEngine)
	{
		return Cast<UDAGGameUserSettings>(GEngine->GetGameUserSettings());
	}

	return nullptr;
}
