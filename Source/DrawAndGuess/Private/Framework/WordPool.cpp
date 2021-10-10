// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/WordPool.h"

void UWordPool::LoadFromFile(FString Path)
{
	TArray<FString> LoadedStrings;
	FFileHelper::LoadFileToStringArray(LoadedStrings, *Path);

	int32 WordCount = 0;
	for (FString LoadedString : LoadedStrings)
	{
		if (!LoadedString.StartsWith("#") && !LoadedString.IsEmpty())
		{
			WordList.AddUnique(FWordInfo{LoadedString});

			UE_LOG(LogInit, Log, TEXT("[WordPool] Word %d: %s"), WordCount, *LoadedString);
			WordCount++;
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
