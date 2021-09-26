#pragma once

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"

#include "SJoyColorPicker.h"
#include "JoyColorWheel.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJoyColorChangedSignature, const FLinearColor&, NewColor);

UCLASS()
class DRAWANDGUESS_API UJoyColorWheel : public UWidget
{
	GENERATED_UCLASS_BODY()
public:

protected:
	TSharedPtr<SJoyColorPicker> ColorPicker;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="JoyColorWheel")
	FLinearColor JoyColor;

	/** Should the color picker jump instantly to the chosen JoyColor when it is first created? */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="JoyColorWheel")
	bool bSkipAnimationOnConstruction = false;

	UPROPERTY(BlueprintAssignable, Category="Widget Event")
	FOnJoyColorChangedSignature OnColorChanged;

	UFUNCTION(BlueprintPure, Category="JoyColorWheel")
	FLinearColor GetColor();

	UFUNCTION(BlueprintCallable, Category="JoyColorWheel")
	void SetColor(FLinearColor NewColor, bool SkipAnimation = false);

public:
	void ColorUpdated(FLinearColor NewValue);
	void ColorPickCancelled(FLinearColor NewValue);

	// UVisual interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	// End of UVisual interface

protected:
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface

#if WITH_EDITOR
	// UObject interface
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	// End of UObject interface
#endif
};
