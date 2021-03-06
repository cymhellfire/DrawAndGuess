// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/DAGGameStateBase.h"
#include "DAGStandardGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDrawingRemainTimeChangedSignature, int32, RemainTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentPlayerIndexChangedSignature, int32, PlayerIndex);

/**
 * 
 */
UCLASS()
class DRAWANDGUESS_API ADAGStandardGameState : public ADAGGameStateBase
{
	GENERATED_BODY()

public:
	void SetRemainTime(int32 NewTime);

	UFUNCTION(BlueprintCallable, Category="GameState")
	int32 GetRemainTime() const { return DrawingRemainTime; }

	void SetCurrentPlayerIndex(int32 NewIndex);

	UFUNCTION(BlueprintCallable, Category="GameState")
	int32 GetCurrentPlayerIndex() const { return CurrentPlayerIndex; }

	void SetDrawerGainScore(int32 NewScore);

	int32 GetDrawerGainScore() const { return DrawerGainScore; }

	void SetGuesserGainScore(int32 NewScore);

	int32 GetGuesserGainScore() const { return GuesserGainScore; }

protected:
	UFUNCTION()
	void OnRep_DrawingRemainTime();

	UFUNCTION()
	void OnRep_CurrentPlayerIndex();

	UFUNCTION()
	void OnRep_DrawerGainScore();

	UFUNCTION()
	void OnRep_GuesserGainScore();

public:
	UPROPERTY(BlueprintAssignable, Category="GameState")
	FOnDrawingRemainTimeChangedSignature OnDrawingRemainTimeChanged;

	UPROPERTY(BlueprintAssignable, Category="GameState")
	FOnCurrentPlayerIndexChangedSignature OnCurrentPlayerIndexChanged;

	UPROPERTY(BlueprintAssignable, Category="GameState")
	FOnCurrentPlayerIndexChangedSignature OnDrawerGainScoreChanged;

	UPROPERTY(BlueprintAssignable, Category="GameState")
	FOnCurrentPlayerIndexChangedSignature OnGuesserGainScoreChanged;

protected:
	UPROPERTY(ReplicatedUsing=OnRep_DrawingRemainTime)
	int32 DrawingRemainTime;
	
	UPROPERTY(ReplicatedUsing=OnRep_CurrentPlayerIndex)
	int32 CurrentPlayerIndex;

	UPROPERTY(ReplicatedUsing=OnRep_DrawerGainScore)
	int32 DrawerGainScore;

	UPROPERTY(ReplicatedUsing=OnRep_GuesserGainScore)
	int32 GuesserGainScore;
};
