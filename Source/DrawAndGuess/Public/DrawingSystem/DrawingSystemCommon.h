#pragma once

#include "CoreMinimal.h"
#include "DrawingSystemCommon.generated.h"

class ADrawingCanvas;

UENUM(BlueprintType)
enum EDrawingActionType
{
	DAT_Pencil,
	DAT_Line,
};

UENUM(BlueprintType)
enum EDrawingInputType
{
	DIE_Pressed,
	DIE_Released,
	DIE_Dragged,
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

struct FDrawingInputEvent
{
	FVector2D InputLocation;
	EDrawingInputType InputType;
	ADrawingCanvas* Canvas;
};

/* Null vector value only for drawing points location comparison. (-1, -1) */
static FVector2D NullVector(-1, -1);
