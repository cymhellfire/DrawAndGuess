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

	UFUNCTION(BlueprintCallable, Category="GameUserSettings")
	void SetPlayerName(const FString& NewName);

	UFUNCTION(BlueprintCallable, Category="GameUserSetttings")
	FString GetPlayerName() const;

	UFUNCTION(BlueprintCallable, Category="GameUserSettings")
	void SetLastRoomIP(const FString& NewIp);

	UFUNCTION(BlueprintCallable, Category="GameUserSettings")
	FString GetLastRoomIP() const { return LastRoomIP; }

	UFUNCTION(BlueprintCallable, Category="GameUserSettings")
	void SetWordPoolFilePath(const FString& NewPath);

	UFUNCTION(BlueprintCallable, Category="GameUserSettings")
	FString GetWordPoolFilePath() const { return WordPoolFilePath; }

	UFUNCTION(BlueprintCallable, Category="GameUserSettings")
	FString GetWordPoolFileFolder() const;

	UFUNCTION(BlueprintCallable, Category="GameInstance")
	static UDAGGameUserSettings* GetDAGGameUserSettings();

protected:
	UPROPERTY(config)
	float PencilInterpolationThreshold;

	UPROPERTY(config)
	FString PlayerName;

	UPROPERTY(config)
	FString LastRoomIP;

	UPROPERTY(config)
	FString WordPoolFilePath;
};
