// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/WordPool.h"

void UWordPool::LoadFromFile(FString Path)
{
	TArray<FString> LoadedStrings;
	FFileHelper::LoadFileToStringArray(LoadedStrings, *Path);

	int32 LineNum = 0;
	for (FString LoadedString : LoadedStrings)
	{
		UE_LOG(LogInit, Log, TEXT("[WordPool] Line %d: %s"), LineNum, *LoadedString);
		LineNum++;

		if (!LoadedString.StartsWith("#"))
		{
			WordList.AddUnique(FWordInfo{LoadedString});
		}
	}
}

const FWordInfo* UWordPool::GetRandomWord() const
{
	if (WordList.Num() == 0)
	{
		return nullptr;
	}

	const int32 WordIndex = FMath::RandRange(0, WordList.Num() - 1);
	return &WordList[WordIndex];
}
