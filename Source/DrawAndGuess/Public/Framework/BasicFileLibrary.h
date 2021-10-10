// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BasicFileLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DRAWANDGUESS_API UBasicFileLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="FileLibrary")
	static bool OpenFileDialog(const FString& Title, const FString& DefaultPath, const FString& FileExtension, TArray<FString>& FilePaths);
};
