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

TArray<const FWordInfo*> UWordPool::GetRandomWord(int32 Count) const
{
	TArray<const FWordInfo*> Candidate;
	// Add all words into Candidate
	for (int32 Index = 0; Index < WordList.Num(); ++Index)
	{
		Candidate.AddUnique(&WordList[Index]);
	}

	if (WordList.Num() <= Count)
	{
		return Candidate;
	}

	TArray<const FWordInfo*> Result;
	// Get random words
	for (int32 i = 0; i < Count; ++i)
	{
		const int32 WordIndex = FMath::RandRange(0, Candidate.Num() - 1);

		Result.AddUnique(Candidate[WordIndex]);
		// Remove picked word from candidate
		Candidate.RemoveAt(WordIndex);
	}

	return Result;
}
