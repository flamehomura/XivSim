// Copyright@Homura Akane@Chocobo


#include "XivSimHotKeyWidget.h"

#include "XivSimActionComponent.h"
#include "XivSimActionManager.h"
#include "XivSimCharacter.h"
#include "Styling\SlateTypes.h"
#include "XivSimHotKeyButtonWidget.h"
#include "XivSimPlayerController.h"

UXivSimHotKeyWidget::UXivSimHotKeyWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UXivSimHotKeyWidget::SetActionDataByID(const int32& NewID)
{
	if(UXivSimActionManager* ActionManager = UXivSimActionManager::Get(GetOwningPlayer()))
	{
		if(const FXivSimActionData* NewData = ActionManager->GetActionData(NewID))
		{
			SetActionData(*NewData);
		}
	}
}

void UXivSimHotKeyWidget::SetActionData(const FXivSimActionData& ActionData)
{
	if(HotKey)
	{
		FButtonStyle CurStyle = HotKey->GetStyle();

		CurStyle.Normal.SetResourceObject(ActionData.ActionIcon);
		CurStyle.Hovered.SetResourceObject(ActionData.ActionIcon);
		CurStyle.Pressed.SetResourceObject(ActionData.ActionIcon);
		CurStyle.Disabled.SetResourceObject(ActionData.ActionIcon);

		HotKey->SetStyle(CurStyle);

		HotKeyActionData = ActionData;
	}
}

void UXivSimHotKeyWidget::BindAction(UInputAction* NewAction)
{
	if(NewAction)
	{
		if(AXivSimPlayerController* PC = GetOwningXivSimPC())
		{
			if(UInputComponent* InputComp = PC->InputComponent)
			{
				if(HotKey)
				{
					HotKey->BindInputAction(InputComp, NewAction);
				}
			}
		}
	}
}

void UXivSimHotKeyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(HotKey)
	{
		HotKey->OnPressed.AddUniqueDynamic(this, &UXivSimHotKeyWidget::OnHotKeyPressed);
	}
}

void UXivSimHotKeyWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	CheckActionAvailable();
}

void UXivSimHotKeyWidget::CheckActionAvailable()
{
	if(HotKey)
	{
		if(HotKeyActionData.IsValid())
		{
			bool bAvailable = false;
			if(UXivSimActionComponent* ActionComponent = GetActionComponent())
			{
				if(AXivSimPlayerController* PC = GetOwningXivSimPC())
				{
					AXivSimCharacterBase* TargetChar = PC->GetCurrentSelectedCharacter();
					bAvailable = ActionComponent->CheckActionTarget(HotKeyActionData, TargetChar);
				}
			}

			HotKey->SetIsEnabled(bAvailable);
		}
	}
}

UXivSimActionComponent* UXivSimHotKeyWidget::GetActionComponent()
{
	if(AXivSimCharacter* Player = GetOwningXivSimPlayer())
	{
		return Player->GetActionComponent();
	}

	return nullptr;
}

bool UXivSimHotKeyWidget::ActionInCoolingDown()
{
	if(HotKeyActionData.IsValid())
	{
		if(UXivSimActionComponent* ActionComponent = GetActionComponent())
		{
			return ActionComponent->CheckActionInCD(HotKeyActionData);
		}
	}

	return false;
}

bool UXivSimHotKeyWidget::ActionIsCasting()
{
	if(HotKeyActionData.IsValid())
	{
		if(UXivSimActionComponent* ActionComponent = GetActionComponent())
		{
			return ActionComponent->IsCasting();
		}
	}

	return false;
}

float UXivSimHotKeyWidget::ActionCoolingDownTimePercent()
{
	if(HotKeyActionData.IsValid())
	{
		if(UXivSimActionComponent* ActionComponent = GetActionComponent())
		{
			return ActionComponent->GetActionCDPercent(HotKeyActionData);
		}
	}

	return 1.f;
}

void UXivSimHotKeyWidget::OnHotKeyPressed()
{
	if(AXivSimCharacter* Player = GetOwningXivSimPlayer())
	{
		if(HotKeyActionData.IsValid())
		{
			if(ActionInCoolingDown() || ActionIsCasting())
			{
				if(UXivSimActionComponent* ActionComponent = Player->GetActionComponent())
				{
					ActionComponent->TryCacheActionInput(HotKeyActionData.ActionID, GetOwningXivSimPC()->GetCurrentSelectedCharacter());
				}
				return;
			}
		
			if(UXivSimActionComponent* ActionComponent = Player->GetActionComponent())
			{
				ActionComponent->TryActivateAction(HotKeyActionData.ActionID, GetOwningXivSimPC()->GetCurrentSelectedCharacter());
			}
		}
	}
}
