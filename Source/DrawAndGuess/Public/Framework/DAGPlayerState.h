// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DrawAndGuessType.h"
#include "GameFramework/PlayerState.h"
#include "DAGPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerLobbyInfoChangedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerGameStateChangedSignature, EPlayerGameState, NewState);

/**
 * 
 */
UCLASS()
class DRAWANDGUESS_API ADAGPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="PlayerState")
	FPlayerLobbyInfoChangedSignature OnPlayerLobbyInfoChanged;

	UPROPERTY(BlueprintAssignable, Category="PlayerState")
	FPlayerGameStateChangedSignature OnPlayerGameStateChanged;

	UFUNCTION(Server, Reliable)
	void SetLobbyState(EPlayerLobbyState NewLobbyState);

	UFUNCTION(BlueprintCallable, Category="PlayerState")
	EPlayerLobbyState GetLobbyState() const { return LobbyState; }

	void SetGameState(EPlayerGameState NewGameState);

	UFUNCTION(BlueprintCallable, Category="PlayerState")
	EPlayerGameState GetGameState() const { return GameState; }

	virtual void OnRep_PlayerName() override;

	void AddDrawScore(int32 DeltaScore);

	void SetDrawScore(int32 NewScore);

	UFUNCTION(BlueprintCallable, Category="PlayerState")
	int32 GetDrawScore() const { return DrawScore; }

protected:
	UFUNCTION()
	void OnRep_LobbyState();

	UFUNCTION()
	void OnRep_GameState();
protected:
	UPROPERTY(ReplicatedUsing=OnRep_LobbyState)
	EPlayerLobbyState LobbyState;

	UPROPERTY(ReplicatedUsing=OnRep_GameState)
	EPlayerGameState GameState;

	UPROPERTY(Replicated)
	int32 DrawScore;
};
