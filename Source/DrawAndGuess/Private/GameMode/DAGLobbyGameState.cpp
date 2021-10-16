// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/DAGLobbyGameState.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

void ADAGLobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParam;
	SharedParam.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS(ADAGLobbyGameState, DrawingTimePerRound, SharedParam);
	DOREPLIFETIME_WITH_PARAMS(ADAGLobbyGameState, MaxDrawRound, SharedParam);
	DOREPLIFETIME_WITH_PARAMS(ADAGLobbyGameState, CandidateWordCount, SharedParam);
}

void ADAGLobbyGameState::SetMaxDrawRound(int32 NewValue)
{
	if (GetNetMode() == NM_Client)
		return;

	if (NewValue != MaxDrawRound)
	{
		MaxDrawRound = NewValue;
		MARK_PROPERTY_DIRTY_FROM_NAME(ADAGLobbyGameState, MaxDrawRound, this);

		OnRep_MaxDrawRound();
	}
}

void ADAGLobbyGameState::SetDrawingTimePerRound(int32 NewValue)
{
	if (GetNetMode() == NM_Client)
		return;

	if (NewValue != DrawingTimePerRound)
	{
		DrawingTimePerRound = NewValue;
		MARK_PROPERTY_DIRTY_FROM_NAME(ADAGLobbyGameState, DrawingTimePerRound, this);

		OnRep_DrawingTimePerRound();
	}
}

void ADAGLobbyGameState::SetCandidateWordCount(int32 NewValue)
{
	if (GetNetMode() == NM_Client)
		return;

	if (NewValue != CandidateWordCount)
	{
		CandidateWordCount = NewValue;
		MARK_PROPERTY_DIRTY_FROM_NAME(ADAGLobbyGameState, CandidateWordCount, this);

		OnRep_CandidateWordCount();
	}
}

void ADAGLobbyGameState::OnRep_DrawingTimePerRound()
{
	OnDrawingTimePerRoundChanged.Broadcast(DrawingTimePerRound);
}

void ADAGLobbyGameState::OnRep_MaxDrawRound()
{
	OnMaxDrawRoundChanged.Broadcast(MaxDrawRound);
}

void ADAGLobbyGameState::OnRep_CandidateWordCount()
{
	OnCandidateWordCountChanged.Broadcast(CandidateWordCount);
}
