#include "Framework/DAGGameStateBase.h"

#include "Framework/DAGGameModeBase.h"
#include "Framework/DAGPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

void ADAGGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParam;
	SharedParam.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS(ADAGGameStateBase, CurrentDrawerName, SharedParam);
}

void ADAGGameStateBase::AddPlayerState(APlayerState* PlayerState)
{
	int32 OrgPlayerCount = PlayerArray.Num();

	Super::AddPlayerState(PlayerState);

	// Check if player count changed
	if (PlayerArray.Num() != OrgPlayerCount)
	{
		OnPlayerStateRegistered.Broadcast(PlayerState);
	}
}

void ADAGGameStateBase::RemovePlayerState(APlayerState* PlayerState)
{
	int32 OrgPlayerCount = PlayerArray.Num();

	Super::RemovePlayerState(PlayerState);

	// Check if player count changed
	if (OrgPlayerCount != PlayerArray.Num())
	{
		OnPlayerStateUnregistered.Broadcast(PlayerState);
	}
}

void ADAGGameStateBase::FinishGameAndReturnToMainMenu()
{
	if (AuthorityGameMode)
	{
		// Invoke the GameMode corresponding method on server side
		ADAGGameModeBase* GameMode = Cast<ADAGGameModeBase>(AuthorityGameMode);
		if (GameMode != nullptr)
		{
			GameMode->RequestFinishAndExitToMainMenu();
		}
	}
	else
	{
		// Only do self job on client side
		ADAGPlayerController* const PlayerController = Cast<ADAGPlayerController>(GetGameInstance()->GetFirstLocalPlayerController());
		if (PlayerController != nullptr)
		{
			PlayerController->HandleReturnToMainMenu();
		}
	}
}

FString ADAGGameStateBase::GetUniquePlayerName(FString PlayerName)
{
	int32 DuplicatedCount = 0;
	FRegexPattern NamePattern(FString::Printf(TEXT("%s(_\\d)*"), *PlayerName));
	for (APlayerState* PlayerState : PlayerArray)
	{
		FString CheckName = PlayerState->GetPlayerName();
		FRegexMatcher Matcher(NamePattern, CheckName);
		if (Matcher.FindNext())
		{
			DuplicatedCount++;
		}
	}

	if (DuplicatedCount > 0)
	{
		return FString::Printf(TEXT("%s_%d"), *PlayerName, DuplicatedCount);
	}

	return PlayerName;
}

void ADAGGameStateBase::AddDrawerName(FString DrawerName)
{
	if (CurrentDrawerName.Len() == 0)
	{
		CurrentDrawerName = DrawerName;
	}
	else
	{
		CurrentDrawerName.Appendf(TEXT("&%s"), *DrawerName);
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ADAGGameStateBase, CurrentDrawerName, this);
}

void ADAGGameStateBase::ClearDrawerName()
{
	CurrentDrawerName = "";

	MARK_PROPERTY_DIRTY_FROM_NAME(ADAGGameStateBase, CurrentDrawerName, this);
}
