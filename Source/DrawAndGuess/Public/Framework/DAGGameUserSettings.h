// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "DAGGameUserSettings.generated.h"

/**
 * 
 */
UCLASS(config=GameUserSettings, configdonotcheckdefaults)
class DRAWANDGUESS_API UDAGGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UDAGGameUserSettings(const FObjectInitializer& ObjectInitializer);

	float GetPencilInterpolateThreshold() const;

protected:
	UPROPERTY(config)
	float PencilInterpolationThreshold;
};
