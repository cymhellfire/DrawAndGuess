// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/DAGGameModeBase.h"
#include "Framework/WordInfo.h"
#include "DAGStandardGameMode.generated.h"

class ADAGStandardGameState;
class UWordPool;

UENUM(BlueprintType)
enum EStandardGameModePhase
{
	SGMP_Waiting,
	SGMP_GameStart,
	SGMP_RoundStart,
	SGMP_ChooseWord,
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

	void ChooseWordByIndex(int32 Index);

protected:
	virtual void BeginPlay() override;

	void SetNextPhase(EStandardGameModePhase NextPhase);

	void SwitchToPhase(EStandardGameModePhase NewPhase);

	void OnGameStarted();

	void OnRoundStarted();

	void OnChooseWord();

	void OnPlayerDrawing();

	void OnRoundEnded();

	void OnSwitchPlayer();

	void OnGameFinished();

	int32 GetPlayerIdByIndex(int32 Index) const;

	int32 GetCurrentPlayerId() const;

	void OnWordGuessed(ADAGPlayerController* PlayerController);

	UFUNCTION()
	void OnDrawingTimerTicked();

	UFUNCTION()
	void OnChooseWordTimerTicked();

	void OnWordChosen();

	virtual void PreBroadcastChatMessage(ADAGPlayerController* SourcePlayer, FString& InMessage) override;

	virtual void OnAllPlayerReady() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="GameMode")
	FString MaskStringForWord;

	UPROPERTY(EditDefaultsOnly, Category="GameMode")
	UCurveFloat* DrawerScoreCurve;

	UPROPERTY(EditDefaultsOnly, Category="GameMode")
	UCurveFloat* GuesserScoreCurve;

	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<EStandardGameModePhase> CurrentGamePhase;

	UPROPERTY(VisibleAnywhere)
	int32 DrawingTimePerRound;

	UPROPERTY(VisibleAnywhere)
	int32 ChooseWordTimePerRound;

	UPROPERTY(VisibleAnywhere)
	int32 MaxDrawingRounds;

	UPROPERTY(Transient)
	UWordPool* WordPool;

	EStandardGameModePhase PendingGamePhase;

	int32 CurrentPlayerId;

	int32 FinishedPlayerThisRound;

	int32 FinishedRound;

	int32 GuessedPlayerCount;

	TArray<ADrawingCanvas*> CanvasList;

	FTimerHandle DrawingTimerHandle;
	FTimerHandle ChooseWordTimerHandle;

	TArray<const FWordInfo*> CandidateWords;

	const FWordInfo* CurrentWord;
};
