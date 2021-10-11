// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WordInfo.h"
#include "WordPool.generated.h"

/**
 * 
 */
UCLASS()
class DRAWANDGUESS_API UWordPool : public UObject
{
	GENERATED_BODY()

public:

	void LoadFromFile(FString Path);

	const FWordInfo* GetRandomWord() const;

	int32 GetWordCount() const { return WordList.Num(); }

protected:

	TArray<FWordInfo> WordList;
};
