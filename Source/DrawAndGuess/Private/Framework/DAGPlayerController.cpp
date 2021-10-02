#include "Framework/DAGPlayerController.h"

#include "Localization.h"
#include "Framework/DAGGameInstance.h"
#include "Framework/DAGGameUserSettings.h"
#include "Framework/DAGPlayerState.h"
#include "GameFramework/PlayerState.h"


void ADAGPlayerController::ClientPreStartGame_Implementation()
{
	// Show loading screen locally
	if (UDAGGameInstance* GameInstance = Cast<UDAGGameInstance>(GetGameInstance()))
	{
		GameInstance->ShowLoadingScreen();
	}
}

void ADAGPlayerController::ClientReturnToMainMenuWithReason_Implementation(EReturnToMainMenuReason::Type Reason)
{
	if (UGameInstance* const GameInstance = GetGameInstance())
	{
		// Remove the error handler first
		if (UDAGGameInstance* DAGGameInstance = Cast<UDAGGameInstance>(GameInstance))
		{
			DAGGameInstance->RemoveNetworkFailureHandler();
		}

		GameInstance->ReturnToMainMenu();

		// Add reason message
		if (UDAGGameInstance* DAGGameInstance = Cast<UDAGGameInstance>(GameInstance))
		{
#if UE_EDITOR
			const FString ErrorContextString = UEnum::GetDisplayValueAsText(Reason).ToString();
#else
			const FString ErrorContextString = FString::Printf(TEXT("NetError_%s"),*UEnum::GetDisplayValueAsText(Reason).ToString());
#endif
			UE_LOG(LogDrawAndGuess, Log, TEXT("DisplayValueAsText: %s"), *ErrorContextString);
			
			DAGGameInstance->AddPendingMessage(FText::FromStringTable(ST_UI_LOC, TEXT("NetErrorTitle")),
				FText::FromStringTable(ST_UI_LOC, ErrorContextString));
		}
	}
	else
	{
		UWorld* const World = GetWorld();
		GEngine->HandleDisconnect(World, World->GetNetDriver());
	}
}

void ADAGPlayerController::ClientUploadPlayerInfo_Implementation()
{
	if (GEngine != nullptr)
	{
		if (UDAGGameUserSettings* UserSettings = Cast<UDAGGameUserSettings>(GEngine->GetGameUserSettings()))
		{
			ServerReceivePlayerInfo(UserSettings->GetPlayerName());
		}
	}
}

void ADAGPlayerController::ServerReceivePlayerInfo_Implementation(const FString& InPlayerName)
{
	// Sync player name with uploaded data
	if (APlayerState* MyPlayerState = GetPlayerState<APlayerState>())
	{
		MyPlayerState->SetPlayerName(InPlayerName);
	}
}

void ADAGPlayerController::HandleReturnToMainMenu()
{
	CleanupSessionOnReturnMain();
}

void ADAGPlayerController::ServerSetLobbyState_Implementation(EPlayerLobbyState NewState)
{
	if (ADAGPlayerState* MyPlayerState = GetPlayerState<ADAGPlayerState>())
	{
		MyPlayerState->SetLobbyState(NewState);
	}
}

void ADAGPlayerController::CleanupSessionOnReturnMain()
{
	const UWorld* World = GetWorld();
	UDAGGameInstance* GameInstance = World != nullptr ? Cast<UDAGGameInstance>(World->GetGameInstance()) : nullptr;
	if (ensure(GameInstance != nullptr))
	{
		// Let game instance handle the session stuff
		GameInstance->CleanupSessionOnReturnToMenu();
	}
}
