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

	virtual void SetToDefaults() override;

	float GetPencilInterpolateThreshold() const;

	FString GetPlayerName() const;

protected:
	UPROPERTY(config)
	float PencilInterpolationThreshold;

	UPROPERTY(config)
	FString PlayerName;
};
