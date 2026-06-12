// Copyright@Homura Akane@Chocobo


#include "XivSimActionBarWidget.h"

#include "XivSimActionBarSlotWidget.h"
#include "XivSimActionManager.h"
#include "XivSimPlayerController.h"
#include "XivSimPlayerState.h"
#include "Components/HorizontalBox.h"

UXivSimActionBarWidget::UXivSimActionBarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UXivSimActionBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AXivSimPlayerController* PC = Cast<AXivSimPlayerController>(GetOwningPlayer()))
	{
		PC->OnPlayerStateChange.AddUniqueDynamic(this, &UXivSimActionBarWidget::OnOwnerPlayerStateChange);
		if (AXivSimPlayerState* PS = PC->GetPlayerState<AXivSimPlayerState>())
		{
			OnOwnerPlayerStateChange(PS);
		}
	}

	if(BarBox)
	{
		TArray<UWidget*> SlotArray = BarBox->GetAllChildren();
		for(int32 i = 0; i < SlotArray.Num(); ++i)
		{
			if(UXivSimActionBarSlotWidget* ActionSlot = Cast<UXivSimActionBarSlotWidget>(SlotArray[i]))
			{
				ActionSlot->SetIndex(i);
				if(InputActionArray.Num() > i)
				{
					ActionSlot->SetBindInputAction(InputActionArray[i]);
				}
			}
		}
	}
}

void UXivSimActionBarWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (AXivSimPlayerController* PC = Cast<AXivSimPlayerController>(GetOwningPlayer()))
	{
		PC->OnPlayerStateChange.RemoveDynamic(this, &UXivSimActionBarWidget::OnOwnerPlayerStateChange);
	}

	Super::NativeDestruct();
}

void UXivSimActionBarWidget::OnOwnerPlayerStateChange(AXivSimPlayerState* PlayerState)
{
	if(OwnerPlayerState != PlayerState)
	{
		if(OwnerPlayerState)
		{
			OwnerPlayerState->OnClassTypeChange.RemoveDynamic(this, &UXivSimActionBarWidget::OnPlayerClassChanged);
		}

		OwnerPlayerState = PlayerState;

		if(OwnerPlayerState)
		{
			OwnerPlayerState->OnClassTypeChange.AddUniqueDynamic(this, &UXivSimActionBarWidget::OnPlayerClassChanged);
			OnPlayerClassChanged(OwnerPlayerState->GetCurrentClassType());
		}
	}
}

void UXivSimActionBarWidget::OnPlayerClassChanged(EXivSimClassType ClassType)
{
	if (UXivSimActionManager* ActionManager = UXivSimActionManager::Get(this))
	{
		TArray<FXivSimActionData> Actions = ActionManager->GetClassActionDatas(ClassType);
		InitActionSlots(Actions);
	}
}

void UXivSimActionBarWidget::InitActionSlots(const TArray<FXivSimActionData>& InActionDataArray)
{
	if(BarBox)
	{
		TArray<UWidget*> SlotArray = BarBox->GetAllChildren();

		for(UWidget* SlotWidget : SlotArray)
		{
			if(UXivSimActionBarSlotWidget* ActionSlot = Cast<UXivSimActionBarSlotWidget>(SlotWidget))
			{
				ActionSlot->SetEmpty();
			}
		}
		
		for(int32 i = 0; i < InActionDataArray.Num(); ++i)
		{
			if(SlotArray.Num() > i)
			{
				if(UXivSimActionBarSlotWidget* ActionSlot = Cast<UXivSimActionBarSlotWidget>(SlotArray[i]))
				{
					ActionSlot->SetActionData(InActionDataArray[i]);
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
}
