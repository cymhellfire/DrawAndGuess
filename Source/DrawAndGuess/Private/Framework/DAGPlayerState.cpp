// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/DAGPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

void ADAGPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS(ADAGPlayerState, LobbyState, SharedParams);
	DOREPLIFETIME_WITH_PARAMS(ADAGPlayerState, GameState, SharedParams);
	DOREPLIFETIME_WITH_PARAMS(ADAGPlayerState, DrawScore, SharedParams);
}

void ADAGPlayerState::SetLobbyState_Implementation(EPlayerLobbyState NewLobbyState)
{
	if (NewLobbyState == LobbyState)
	{
		return;
	}

	LobbyState = NewLobbyState;
	MARK_PROPERTY_DIRTY_FROM_NAME(ADAGPlayerState, LobbyState, this);

	// Invoke event on server side
	if (GetNetMode() != NM_Client)
	{
		// Broadcast the event
		OnPlayerLobbyInfoChanged.Broadcast();
	}
}

void ADAGPlayerState::SetGameState(EPlayerGameState NewGameState)
{
	if (GetNetMode() == NM_Client)
		return;

	if (NewGameState != GameState)
	{
		GameState = NewGameState;
		MARK_PROPERTY_DIRTY_FROM_NAME(ADAGPlayerState, GameState, this);

		OnRep_GameState();
	}
}

void ADAGPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

	// Notify the info changes
	OnPlayerLobbyInfoChanged.Broadcast();
}

void ADAGPlayerState::AddDrawScore(int32 DeltaScore)
{
	if (GetNetMode() == NM_Client)
		return;

	DrawScore += DeltaScore;
	MARK_PROPERTY_DIRTY_FROM_NAME(ADAGPlayerState, DrawScore, this);
}

void ADAGPlayerState::SetDrawScore(int32 NewScore)
{
	if (GetNetMode() == NM_Client)
		return;

	if (DrawScore != NewScore)
	{
		DrawScore = NewScore;
		MARK_PROPERTY_DIRTY_FROM_NAME(ADAGPlayerState, DrawScore, this);
	}
}

void ADAGPlayerState::OnRep_LobbyState()
{
	// Broadcast the event
	OnPlayerLobbyInfoChanged.Broadcast();
}

void ADAGPlayerState::OnRep_GameState()
{
	// Broadcast the event
	OnPlayerGameStateChanged.Broadcast(GameState);
}
