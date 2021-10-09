#pragma once

#include "CoreMinimal.h"
#include "DAGChatMessage.generated.h"

USTRUCT(BlueprintType)
struct FDAGChatMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Author;

	UPROPERTY(BlueprintReadOnly)
	FString Content;
};
