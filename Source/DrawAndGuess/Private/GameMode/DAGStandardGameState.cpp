// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/DAGStandardGameState.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

void ADAGStandardGameState::SetRemainTime(int32 NewTime)
{
	if (GetNetMode() == NM_Client)
		return;

	if (DrawingRemainTime != NewTime)
	{
		DrawingRemainTime = NewTime;

		MARK_PROPERTY_DIRTY_FROM_NAME(ADAGStandardGameState, DrawingRemainTime, this);

		OnRep_DrawingRemainTime();
	}
}

void ADAGStandardGameState::SetCurrentPlayerIndex(int32 NewIndex)
{
	if (GetNetMode() == NM_Client)
		return;

	if (CurrentPlayerIndex != NewIndex)
	{
		CurrentPlayerIndex = NewIndex;

		MARK_PROPERTY_DIRTY_FROM_NAME(ADAGStandardGameState, CurrentPlayerIndex, this);

		OnRep_CurrentPlayerIndex();
	}
}

void ADAGStandardGameState::OnRep_DrawingRemainTime()
{
	OnDrawingRemainTimeChanged.Broadcast(DrawingRemainTime);
}

void ADAGStandardGameState::OnRep_CurrentPlayerIndex()
{
	OnCurrentPlayerIndexChanged.Broadcast(CurrentPlayerIndex);
}

void ADAGStandardGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParam;
	SharedParam.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS(ADAGStandardGameState, DrawingRemainTime, SharedParam);
	DOREPLIFETIME_WITH_PARAMS(ADAGStandardGameState, CurrentPlayerIndex, SharedParam);
}
