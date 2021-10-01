#pragma once

#include "DrawAndGuessType.generated.h"

UENUM()
namespace EReturnToMainMenuReason
{
	enum Type
	{
		Unknown			UMETA(DisplayName="NetError_Unknown"),
		LostConnect		UMETA(DisplayName="NetError_LostConnect"),
		HostLeft		UMETA(DisplayName="NetError_HostLeft"),
	};
}

UENUM(BlueprintType)
enum class EPlayerLobbyState : uint8
{
	PLS_Waiting,
	PLS_Ready,
};
