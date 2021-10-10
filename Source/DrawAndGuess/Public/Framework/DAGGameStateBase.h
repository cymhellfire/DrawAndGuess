#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DAGGameStateBase.generated.h"

UCLASS()
class ADAGGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateRegisterSignature, APlayerState*, NewPlayerState);
	UPROPERTY(BlueprintAssignable, Category="GameState")
	FOnPlayerStateRegisterSignature OnPlayerStateRegistered;

	UPROPERTY(BlueprintAssignable, Category="GameState")
	FOnPlayerStateRegisterSignature OnPlayerStateUnregistered;

	virtual void AddPlayerState(APlayerState* PlayerState) override;

	virtual void RemovePlayerState(APlayerState* PlayerState) override;

	void FinishGameAndReturnToMainMenu();

	FString GetUniquePlayerName(FString PlayerName);
};
