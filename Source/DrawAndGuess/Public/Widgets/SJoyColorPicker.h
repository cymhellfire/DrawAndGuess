#pragma once

#include "Widgets/Colors/SColorPicker.h"

class SJoyColorPicker : public SColorPicker
{
	typedef SColorPicker Super;

public:

	FORCEINLINE void SetColorRGB(const FLinearColor& NewColor)
	{
		SetNewTargetColorRGB(NewColor, true);
	}

public:

	FLinearColor InstantColor;
	bool Animation_SkipToFinalForOneTick = false;

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override
	{
		//Skip to final, then resume normal animation behavior
		if (Animation_SkipToFinalForOneTick)
		{
			Animation_SkipToFinalForOneTick = false;
			Super::Tick(AllottedGeometry, InCurrentTime, 10000);
			SetNewTargetColorRGB(InstantColor, true);
			return;
		}

		// Animate normally
		Super::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	}
};
