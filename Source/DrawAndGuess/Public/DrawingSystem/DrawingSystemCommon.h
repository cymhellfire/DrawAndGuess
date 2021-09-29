#pragma once

#include "CoreMinimal.h"
#include "DrawingSystemCommon.generated.h"

UENUM(BlueprintType)
enum EDrawingActionType
{
	DAT_Pencil,
	DAT_Line,
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

/* Null vector value only for drawing points location comparison. (-1, -1) */
static FVector2D NullVector(-1, -1);
