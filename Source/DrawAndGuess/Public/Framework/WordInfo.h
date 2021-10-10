#pragma once

#include "CoreMinimal.h"

struct FWordInfo
{
	FString Word;

	bool IsMatch(FString TestString) const
	{
		return TestString == Word;
	}

	friend bool operator==(const FWordInfo& Lhs, const FWordInfo& RHS)
	{
		return Lhs.Word == RHS.Word;
	}

	friend bool operator!=(const FWordInfo& Lhs, const FWordInfo& RHS)
	{
		return !(Lhs == RHS);
	}
};
