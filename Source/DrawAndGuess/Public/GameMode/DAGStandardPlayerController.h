// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/DAGPlayerController.h"
#include "DAGStandardPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DRAWANDGUESS_API ADAGStandardPlayerController : public ADAGPlayerController
{
	GENERATED_BODY()

public:

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="PlayerController")
	void ServerChooseWord(int32 Index);

	UFUNCTION(Client, Reliable)
	void ClientOnWordChosen();

protected:

	UFUNCTION(BlueprintImplementableEvent, Category="PlayerController", meta=(DisplayName="OnWordChosen"))
	void K2_OnWordChosen();
};
