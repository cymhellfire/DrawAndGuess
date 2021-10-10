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
	PLS_Waiting			UMETA(DisplayName="Waiting"),
	PLS_Ready			UMETA(DisplayName="Ready"),
	PLS_Host			UMETA(DisplayName="Host"),
};

UENUM(BlueprintType)
enum class EPlayerGameState : uint8
{
	PGS_Guessing,
	PGS_Drawing,
};
