#include "Widgets/JoyColorWheel.h"

#define LOCTEXT_NAMESPACE "UMG"

UJoyColorWheel::UJoyColorWheel(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, JoyColor(FLinearColor::Black)
{
	//Default Values Set Here, see above
}

//Rebuild
TSharedRef<SWidget> UJoyColorWheel::RebuildWidget()
{
	FColorPickerArgs Args;

	//Initial Color
	Args.InitialColorOverride = JoyColor;

	Args.bUseAlpha = true;
	Args.bOnlyRefreshOnOk = false;
	Args.bOnlyRefreshOnMouseUp = false;

	//Delegates!
	Args.OnColorCommitted = FOnLinearColorValueChanged::CreateUObject(this, &UJoyColorWheel::ColorUpdated);
	Args.OnColorPickerCancelled = FOnColorPickerCancelled::CreateUObject(this, &UJoyColorWheel::ColorPickCancelled);


	ColorPicker = SNew(SJoyColorPicker)
		.TargetColorAttribute(Args.InitialColorOverride)
		.TargetFColors(Args.ColorArray ? *Args.ColorArray : TArray<FColor*>())
		.TargetLinearColors(Args.LinearColorArray ? *Args.LinearColorArray : TArray<FLinearColor*>())
		.TargetColorChannels(Args.ColorChannelsArray ? *Args.ColorChannelsArray : TArray<FColorChannels>())
		.UseAlpha(Args.bUseAlpha)
		.ExpandAdvancedSection(Args.bExpandAdvancedSection)
		.OnlyRefreshOnMouseUp(Args.bOnlyRefreshOnMouseUp && !Args.bIsModal)
		.OnlyRefreshOnOk(Args.bOnlyRefreshOnOk || Args.bIsModal)
		.OnColorCommitted(Args.OnColorCommitted)
		.PreColorCommitted(Args.PreColorCommitted)
		.OnColorPickerCancelled(Args.OnColorPickerCancelled)
		.OnInteractivePickBegin(Args.OnInteractivePickBegin)
		.OnInteractivePickEnd(Args.OnInteractivePickEnd)
		.DisplayGamma(Args.DisplayGamma);

	//Skip Animation?
	if (bSkipAnimationOnConstruction)
	{
		SetColor(JoyColor, true); //Skip
	}

	return ColorPicker.ToSharedRef();
}

//Release
void UJoyColorWheel::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	ColorPicker.Reset();
}

#if WITH_EDITOR
void UJoyColorWheel::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != NULL)
		                     ? PropertyChangedEvent.Property->GetFName()
		                     : NAME_None;

	//Update Picker to JoyColor property change!
	if (PropertyName == TEXT("JoyColor"))
	{
		if (ColorPicker.IsValid())
		{
			ColorPicker->SetColorRGB(JoyColor);
		}
	}
}
#endif

//~~~~~~~~~~~~~~~~~~
// BP Exposed
//~~~~~~~~~~~~~~~~~~
FLinearColor UJoyColorWheel::GetColor()
{
	return JoyColor;
}

void UJoyColorWheel::SetColor(FLinearColor NewColor, bool SkipAnimation)
{
	if (!ColorPicker.IsValid()) return;

	//Skip Anim?
	if (SkipAnimation)
	{
		ColorPicker->InstantColor = NewColor;
		ColorPicker->Animation_SkipToFinalForOneTick = true; //See SJoyColorPicker.h
	}
	else
	{
		//Set!
		ColorPicker->SetColorRGB(NewColor);
	}
}

//~~~~~~~~~~~~~~~~~~~~
// Color Picker Internal
//~~~~~~~~~~~~~~~~~~~~
void UJoyColorWheel::ColorUpdated(FLinearColor NewValue)
{
	JoyColor = NewValue;

	if (OnColorChanged.IsBound())
	{
		OnColorChanged.Broadcast(JoyColor);
	}
}

void UJoyColorWheel::ColorPickCancelled(FLinearColor NewValue)
{
	//Color Picking Cancelled!
}


#undef LOCTEXT_NAMESPACE
