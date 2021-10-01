// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DrawAndGuessType.h"
#include "GameFramework/PlayerState.h"
#include "DAGPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class DRAWANDGUESS_API ADAGPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerLobbyInfoChangedSignature);
	FPlayerLobbyInfoChangedSignature OnPlayerLobbyInfoChanged;

	UFUNCTION(Server, Reliable)
	void SetLobbyState(EPlayerLobbyState NewLobbyState);

	UFUNCTION(BlueprintCallable, Category="PlayerState")
	EPlayerLobbyState GetLobbyState() const { return LobbyState; }

	virtual void SetPlayerName(const FString& S) override;

	UFUNCTION()
	void OnRep_LobbyState(EPlayerLobbyState NewState);

	UFUNCTION()
	void OnRep_NetRequestCount(int32 NewCount);
protected:
	UPROPERTY(ReplicatedUsing="OnRep_LobbyState")
	EPlayerLobbyState LobbyState;

	UPROPERTY(ReplicatedUsing=OnRep_NetRequestCount)
	int32 NetRequestCount;
};
