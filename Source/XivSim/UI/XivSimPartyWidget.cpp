#include "XivSimPartyWidget.h"
#include "XivSimGameState.h"
#include "XivSimPlayerState.h"
#include "XivSimPlayerManager.h"
#include "XivSimMemberWidget.h"
#include "Components/PanelWidget.h"
#include "XivSimPlayerController.h"
#include "XivSimPlayerState.h"
#include "TimerManager.h"
#include "XivSimGameUserSettings.h"

UXivSimPartyWidget::UXivSimPartyWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UXivSimPartyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AXivSimPlayerController* PC = Cast<AXivSimPlayerController>(GetOwningPlayer()))
	{
		PC->OnPlayerStateChange.AddUniqueDynamic(this, &UXivSimPartyWidget::OnOwnerPlayerStateChange);
		if (AXivSimPlayerState* PS = PC->GetPlayerState<AXivSimPlayerState>())
		{
			OnOwnerPlayerStateChange(PS);
		}
	}
}

void UXivSimPartyWidget::NativeDestruct()
{
	if (UXivSimPlayerManager* PlayerManager = UXivSimPlayerManager::Get(this))
	{
		PlayerManager->OnPlayerStateAdd.RemoveDynamic(this, &UXivSimPartyWidget::OnPlayerStateAdd);
		PlayerManager->OnPlayerStateRemove.RemoveDynamic(this, &UXivSimPartyWidget::OnPlayerStateRemove);
	}

	if (AXivSimPlayerController* PC = Cast<AXivSimPlayerController>(GetOwningPlayer()))
	{
		if (AXivSimPlayerState* PS = PC->GetPlayerState<AXivSimPlayerState>())
		{
			PS->OnPositionTypeChange.RemoveDynamic(this, &UXivSimPartyWidget::OnPlayerPositionChanged);
		}
		
		PC->OnPlayerStateChange.RemoveDynamic(this, &UXivSimPartyWidget::OnOwnerPlayerStateChange);
	}

	Super::NativeDestruct();
}

void UXivSimPartyWidget::RebuildMemberWidgets()
{
	if (MemberWidgetClass)
	{
		if (UXivSimPlayerManager* PlayerManager = UXivSimPlayerManager::Get(this))
		{
			for (int32 i = 0; i < PlayerManager->GetPlayerStateArray().Num(); ++i)
			{
				if (AXivSimPlayerState* PS = PlayerManager->GetPlayerStateArray()[i])
				{
					OnPlayerStateAdd(PS);
				}
			}

			if (!PlayerManager->OnPlayerStateAdd.IsAlreadyBound(this, &UXivSimPartyWidget::OnPlayerStateAdd))
			{
				PlayerManager->OnPlayerStateAdd.AddUniqueDynamic(this, &UXivSimPartyWidget::OnPlayerStateAdd);
			}
			if (!PlayerManager->OnPlayerStateRemove.IsAlreadyBound(this, &UXivSimPartyWidget::OnPlayerStateRemove))
			{
				PlayerManager->OnPlayerStateRemove.AddUniqueDynamic(this, &UXivSimPartyWidget::OnPlayerStateRemove);
			}
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &UXivSimPartyWidget::RebuildMemberWidgets, 0.1f, false);
		}
	}
}

void UXivSimPartyWidget::OnPlayerStateAdd(class AXivSimPlayerState* PlayerState)
{
	if (PlayerState && !MemberWidgetMap.Find(PlayerState))
	{
		UXivSimMemberWidget* MemberWidget = CreateWidget<UXivSimMemberWidget>(GetOwningPlayer(), MemberWidgetClass);
		if (MemberWidget)
		{
			MemberWidget->SetPlayerState(PlayerState);
			PartyPanel->AddChild(MemberWidget);
			MemberWidgetMap.Add(PlayerState, MemberWidget);

			PlayerState->OnPositionTypeChange.AddUniqueDynamic(this, &UXivSimPartyWidget::OnPlayerPositionChanged);
			if(UXivSimGameUserSettings* GUS = UXivSimGameUserSettings::GetXivSimGameUserSettings())
			{
				SortPartyList(GUS->GetLocalPartyListPositiveSort(), GUS->GetLocalPartyListLocalFirst());
			}
			else
			{
				SortPartyList(true, true);
			}
		}
	}
}

void UXivSimPartyWidget::OnPlayerStateRemove(class AXivSimPlayerState* PlayerState)
{
	if (PlayerState)
	{
		if (TObjectPtr<UXivSimMemberWidget>* MemberWidgetPtr = MemberWidgetMap.Find(PlayerState))
		{
			PlayerState->OnPositionTypeChange.RemoveDynamic(this, &UXivSimPartyWidget::OnPlayerPositionChanged);
			PartyPanel->RemoveChild(*MemberWidgetPtr);
			MemberWidgetMap.Remove(PlayerState);

			if(UXivSimGameUserSettings* GUS = UXivSimGameUserSettings::GetXivSimGameUserSettings())
			{
				SortPartyList(GUS->GetLocalPartyListPositiveSort(), GUS->GetLocalPartyListLocalFirst());
			}
			else
			{
				SortPartyList(true, true);
			}
		}
	}
}

void UXivSimPartyWidget::OnOwnerPlayerStateChange(class AXivSimPlayerState* PlayerState)
{
	if (PlayerState)
	{
		PartyPanel->ClearChildren();
		MemberWidgetMap.Reset();
		OnPlayerStateAdd(PlayerState);
		RebuildMemberWidgets();
	}
}

void UXivSimPartyWidget::OnPlayerPositionChanged(EXivSimPositionType PosType)
{
	if(UXivSimGameUserSettings* GUS = UXivSimGameUserSettings::GetXivSimGameUserSettings())
	{
		SortPartyList(GUS->GetLocalPartyListPositiveSort(), GUS->GetLocalPartyListLocalFirst());
	}
	else
	{
		SortPartyList(true, true);
	}
}

void UXivSimPartyWidget::SortPartyList(bool bPositive, bool bLocalFirst)
{
	struct FPSWidget
	{
		AXivSimPlayerState* PS;
		UXivSimMemberWidget* Widget;

		FPSWidget(AXivSimPlayerState* InPS, UXivSimMemberWidget* InWidget)
		{
			PS = InPS;
			Widget = InWidget;
		}
	};

	if(UXivSimGameUserSettings* GUS = UXivSimGameUserSettings::GetXivSimGameUserSettings())
	{
		GUS->SetLocalPartyListPositiveSort(bPositive);
		GUS->SetLocalPartyListLocalFirst(bLocalFirst);
	}
	
	TArray<FPSWidget> SortedWidgetArray;
	for(auto WidgetPair : MemberWidgetMap)
	{
		if(WidgetPair.Key)
		{
			SortedWidgetArray.Add(FPSWidget(WidgetPair.Key, WidgetPair.Value));
		}
	}

	SortedWidgetArray.Sort([=](const FPSWidget& PSW1, const FPSWidget& PSW2)
		{
			if(bPositive)
			{
				return PSW1.PS->GetCurrentPositionType() < PSW2.PS->GetCurrentPositionType();
			}
			else
			{
				return PSW1.PS->GetCurrentPositionType() > PSW2.PS->GetCurrentPositionType();
			}
		});

	if(bLocalFirst)
	{
		AXivSimPlayerState* LocalPS = GetOwningXivSimPC()->GetPlayerState<AXivSimPlayerState>();
		for(int32 i = 0; i < SortedWidgetArray.Num(); ++i)
		{
			if(SortedWidgetArray[i].PS == LocalPS)
			{
				FPSWidget CachedPSW = SortedWidgetArray[i];
				SortedWidgetArray.RemoveAt(i);
				SortedWidgetArray.Insert(CachedPSW, 0);
				break;
			}
		}
	}
	
	PartyPanel->ClearChildren();
	for(int32 i = 0; i < SortedWidgetArray.Num(); ++i)
	{
		PartyPanel->AddChild(SortedWidgetArray[i].Widget);
	}
}
