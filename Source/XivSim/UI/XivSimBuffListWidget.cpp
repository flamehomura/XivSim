#include "XivSimBuffListWidget.h"
#include "XivSimBuffWidget.h"
#include "XivSimCharacter.h"
#include "BuffComponent.h"
#include "XivSimBuffBase.h"
#include "Components/PanelWidget.h"

UXivSimBuffListWidget::UXivSimBuffListWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UXivSimBuffListWidget::SetPlayerPawn(class AXivSimCharacter* NewPawn)
{
	if (NewPawn != PlayerPawn)
	{
		if (PlayerPawn)
		{
			PlayerPawn->GetBuffComponent()->OnBuffAdd.RemoveDynamic(this, &UXivSimBuffListWidget::OnBuffAdd);
			PlayerPawn->GetBuffComponent()->OnBuffRemove.RemoveDynamic(this, &UXivSimBuffListWidget::OnBuffRemove);
		}

		PlayerPawn = NewPawn;
		BuffWidgetMap.Reset();
		ListPanel->ClearChildren();

		if (PlayerPawn)
		{
			const FBuffInfoContainer& BuffContainer = PlayerPawn->GetBuffComponent()->GetBuffContainer();
			for (int32 i = 0; i < BuffContainer.BuffList.Num(); ++i)
			{
				OnBuffAdd(BuffContainer.BuffList[i].BuffPtr);
			}

			PlayerPawn->GetBuffComponent()->OnBuffAdd.AddUniqueDynamic(this, &UXivSimBuffListWidget::OnBuffAdd);
			PlayerPawn->GetBuffComponent()->OnBuffRemove.AddUniqueDynamic(this, &UXivSimBuffListWidget::OnBuffRemove);
		}
	}
}

void UXivSimBuffListWidget::OnBuffAdd(class UXivSimBuffBase* InBuff)
{
	if (InBuff && !BuffWidgetMap.Find(InBuff))
	{
		if (!InBuff->GetBuffIcon())
		{
			return;
		}
		UXivSimBuffWidget* BuffWidget = CreateWidget<UXivSimBuffWidget>(GetOwningPlayer(), BuffWidgetClass);
		if (BuffWidget)
		{
			BuffWidget->SetBuffPtr(InBuff);
			if (InBuff->IsAttentionBuff())
			{
				TArray<UWidget*> Children = ListPanel->GetAllChildren();
				Children.Insert(BuffWidget, 0);
				ListPanel->ClearChildren();
				for (UWidget* Widget : Children)
				{
					ListPanel->AddChild(Widget);
				}
			}
			else
			{
				ListPanel->AddChild(BuffWidget);
			}
			BuffWidgetMap.Add(InBuff, BuffWidget);
		}
	}
}

void UXivSimBuffListWidget::OnBuffRemove(class UXivSimBuffBase* InBuff)
{
	if (InBuff)
	{
		if (TObjectPtr<UXivSimBuffWidget>* BuffWidgetPtr = BuffWidgetMap.Find(InBuff))
		{
			ListPanel->RemoveChild(*BuffWidgetPtr);
			BuffWidgetMap.Remove(InBuff);
		}
	}
}
