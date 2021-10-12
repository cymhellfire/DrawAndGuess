// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/DAGGameModeBase.h"
#include "DAGTestGameMode.generated.h"

/**
 * 
 */
UCLASS()
class DRAWANDGUESS_API ADAGTestGameMode : public ADAGGameModeBase
{
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
