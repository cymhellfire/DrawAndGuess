#pragma once

#include "CoreMinimal.h"
#include "DrawingSystemCommon.generated.h"

UENUM(BlueprintType)
enum EDrawingActionType
{
	DAT_Pencil,
};

USTRUCT(BlueprintType)
struct DRAWANDGUESS_API FDrawingBrushSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* BrushTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BrushSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor BrushColor;
};
