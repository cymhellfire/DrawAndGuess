// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/DAGGameStateBase.h"
#include "DAGLobbyGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameSettingIntPropertyChanged, int32, NewValue);

/**
 * 
 */
UCLASS()
class DRAWANDGUESS_API ADAGLobbyGameState : public ADAGGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category=GameState)
	FOnGameSettingIntPropertyChanged OnDrawingTimePerRoundChanged;

	UPROPERTY(BlueprintAssignable, Category=GameState)
	FOnGameSettingIntPropertyChanged OnMaxDrawRoundChanged;

	UPROPERTY(BlueprintAssignable, Category=GameState)
	FOnGameSettingIntPropertyChanged OnCandidateWordCountChanged;

	UFUNCTION(BlueprintCallable, Category=GameState)
	void SetMaxDrawRound(int32 NewValue);

	UFUNCTION(BlueprintCallable, Category=GameState)
	int32 GetMaxDrawRound() const { return MaxDrawRound; }

	UFUNCTION(BlueprintCallable, Category=GameState)
	void SetDrawingTimePerRound(int32 NewValue);

	UFUNCTION(BlueprintCallable, Category=GameState)
	int32 GetDrawingTimePerRound() const { return DrawingTimePerRound; }

	UFUNCTION(BlueprintCallable, Category=GameState)
	void SetCandidateWordCount(int32 NewValue);

	UFUNCTION(BlueprintCallable, Category=GameState)
	int32 GetCandidateWordCount() const { return CandidateWordCount; }

protected:
	UFUNCTION()
	void OnRep_DrawingTimePerRound();

	UFUNCTION()
	void OnRep_MaxDrawRound();

	UFUNCTION()
	void OnRep_CandidateWordCount();

protected:

	// Following variables are not used in actual game, they are only used to replicate game settings to all clients.
	UPROPERTY(ReplicatedUsing=OnRep_DrawingTimePerRound)
	int32 DrawingTimePerRound;

	UPROPERTY(ReplicatedUsing=OnRep_MaxDrawRound)
	int32 MaxDrawRound;

	UPROPERTY(ReplicatedUsing=OnRep_CandidateWordCount)
	int32 CandidateWordCount;
};
