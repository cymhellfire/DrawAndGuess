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
	DOREPLIFETIME_WITH_PARAMS(ADAGPlayerState, NetRequestCount, SharedParams);
}

void ADAGPlayerState::SetLobbyState_Implementation(EPlayerLobbyState NewLobbyState)
{
	if (NewLobbyState == LobbyState)
	{
		return;
	}

	LobbyState = NewLobbyState;
	MARK_PROPERTY_DIRTY_FROM_NAME(ADAGPlayerState, LobbyState, this);

	UE_LOG(LogInit, Log, TEXT("Server set LobbyState to %s"), *UEnum::GetValueAsString<EPlayerLobbyState>(NewLobbyState));

	NetRequestCount ++;
	MARK_PROPERTY_DIRTY_FROM_NAME(ADAGPlayerState, NetRequestCount, this);

	// Broadcast the event
	OnPlayerLobbyInfoChanged.Broadcast();
}

void ADAGPlayerState::SetPlayerName(const FString& S)
{
	Super::SetPlayerName(S);

	// Broadcast rename event
	OnPlayerLobbyInfoChanged.Broadcast();
}

void ADAGPlayerState::OnRep_LobbyState(EPlayerLobbyState NewState)
{
	UE_LOG(LogInit, Log, TEXT("OnRep_LobbyState to %s"), *UEnum::GetValueAsString<EPlayerLobbyState>(NewState));
}

void ADAGPlayerState::OnRep_NetRequestCount(int32 NewCount)
{
	UE_LOG(LogInit, Log, TEXT("OnRep_NetRequestCount: %d"), NewCount);
}
