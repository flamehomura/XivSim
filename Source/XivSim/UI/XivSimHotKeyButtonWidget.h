// Copyright@Homura Akane@Chocobo

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Widgets/Input/SButton.h"
#include "XivSimHotKeyButtonWidget.generated.h"

/**
 * 
 */
class SXivSimHotKeyButton : public SButton
{
public:
	void CustomPress();
	void CustomRelease();
};

UCLASS()
class XIVSIM_API UXivSimHotKeyButtonWidget : public UButton
{
	GENERATED_BODY()

public:
	void BindInputAction(class UInputComponent* InputComponent, class UInputAction* InputAction);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	
	UFUNCTION()
	void InputKeyPressed();
	UFUNCTION()
	void InputKeyReleased();
};
