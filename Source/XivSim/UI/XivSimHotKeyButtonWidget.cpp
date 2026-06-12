// Copyright@Homura Akane@Chocobo


#include "XivSimHotKeyButtonWidget.h"

#include "EnhancedInputComponent.h"
#include "Components/ButtonSlot.h"

void SXivSimHotKeyButton::CustomPress()
{
	Press();
}

void SXivSimHotKeyButton::CustomRelease()
{
	Release();
}

void UXivSimHotKeyButtonWidget::BindInputAction(UInputComponent* InputComponent, UInputAction* InputAction)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->ClearBindingsForObject(this);
		EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Started, this, &UXivSimHotKeyButtonWidget::InputKeyPressed);
		EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Completed, this, &UXivSimHotKeyButtonWidget::InputKeyReleased);
	}
}

TSharedRef<SWidget> UXivSimHotKeyButtonWidget::RebuildWidget()
{
PRAGMA_DISABLE_DEPRECATION_WARNINGS
	MyButton = SNew(SXivSimHotKeyButton)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClicked))
		.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandlePressed))
		.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleReleased))
		.OnHovered_UObject( this, &ThisClass::SlateHandleHovered )
		.OnUnhovered_UObject( this, &ThisClass::SlateHandleUnhovered )
		.ButtonStyle(&WidgetStyle)
		.ClickMethod(ClickMethod)
		.TouchMethod(TouchMethod)
		.PressMethod(PressMethod)
		.IsFocusable(IsFocusable)
		;
PRAGMA_ENABLE_DEPRECATION_WARNINGS
		if ( GetChildrenCount() > 0 )
		{
			Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyButton.ToSharedRef());
		}
	
	return MyButton.ToSharedRef();
}

void UXivSimHotKeyButtonWidget::InputKeyPressed()
{
	if(SXivSimHotKeyButton* SHotKey = static_cast<SXivSimHotKeyButton*>(MyButton.Get()))
	{
		SHotKey->CustomPress();
	}
}

void UXivSimHotKeyButtonWidget::InputKeyReleased()
{
	if(SXivSimHotKeyButton* SHotKey = static_cast<SXivSimHotKeyButton*>(MyButton.Get()))
	{
		SHotKey->CustomRelease();
	}
}
