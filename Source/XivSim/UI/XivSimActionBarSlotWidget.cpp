// Copyright@Homura Akane@Chocobo


#include "XivSimActionBarSlotWidget.h"

#include "InputAction.h"
#include "XivSimHotKeyWidget.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"


UXivSimActionBarSlotWidget::UXivSimActionBarSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActionButtonWidget = nullptr;
	SlotIndex = -1;
	BindAction = nullptr;
}

void UXivSimActionBarSlotWidget::SetIndex(int32 NewIdx)
{
	SlotIndex = NewIdx;
}

void UXivSimActionBarSlotWidget::SetBindInputAction(UInputAction* NewAction)
{
	BindAction = NewAction;
	if(SlotText)
	{
		if(BindAction)
		{
			SlotText->SetText(BindAction->ActionDescription);
		}
		else
		{
			SlotText->SetText(FText());
		}
	}
}

void UXivSimActionBarSlotWidget::SetEmpty()
{
	if(ActionBox)
	{
		ActionButtonWidget = nullptr;
		ActionBox->ClearChildren();
	}
}

void UXivSimActionBarSlotWidget::SetActionData(const FXivSimActionData& InData)
{
	SetEmpty();

	if(ActionButtonWidgetClass)
	{
		ActionButtonWidget = CreateWidget<UXivSimHotKeyWidget>(GetOwningPlayer(), ActionButtonWidgetClass);
		if(ActionButtonWidget)
		{
			ActionButtonWidget->SetActionData(InData);
			ActionButtonWidget->BindAction(BindAction);
			ActionBox->AddChild(ActionButtonWidget);
		}
	}
}
