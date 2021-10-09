// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/DAGGameModeBase.h"
#include "DAGStandardGameMode.generated.h"

UENUM(BlueprintType)
enum EStandardGameModePhase
{
	SGMP_Waiting,
	SGMP_GameStart,
	SGMP_RoundStart,
	SGMP_PlayerDrawing,
	SGMP_RoundEnd,
	SGMP_SwitchPlayer,
	SGMP_GameFinish,
};

/**
 * 
 */
UCLASS()
class DRAWANDGUESS_API ADAGStandardGameMode : public ADAGGameModeBase
{
	GENERATED_BODY()

public:
	ADAGStandardGameMode(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

	void SetNextPhase(EStandardGameModePhase NextPhase);

	void SwitchToPhase(EStandardGameModePhase NewPhase);

	void OnGameStarted();

	void OnRoundStarted();

	void OnPlayerDrawing();

	void OnRoundEnded();

	void OnSwitchPlayer();

	void OnGameFinished();

	int32 GetPlayerIdByIndex(int32 Index) const;

	UFUNCTION()
	void OnDrawingTimerExpired();

protected:
	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<EStandardGameModePhase> CurrentGamePhase;

	UPROPERTY(VisibleAnywhere)
	int32 DrawingTimePerRound;

	UPROPERTY(VisibleAnywhere)
	int32 MaxDrawingRounds;

	EStandardGameModePhase PendingGamePhase;

	int32 CurrentPlayerId;

	int32 CurrentPlayerIndex;

	int32 FinishedPlayerThisRound;

	int32 FinishedRound;

	TArray<ADrawingCanvas*> CanvasList;

	FTimerHandle DrawingTimerHandle;
};
