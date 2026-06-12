// Copyright@赤音焰@Bilibili


#include "XivSimGimmickCastingListWidget.h"
#include "XivSimGimmickCastingBarWidget.h"
#include "XivSimGameState.h"
#include "XivSimPatternBase.h"
#include "Components/VerticalBox.h"

UXivSimGimmickCastingListWidget::UXivSimGimmickCastingListWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UXivSimGimmickCastingListWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	InitPatternDelegate();
}

void UXivSimGimmickCastingListWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	if (AXivSimGameState* GameState = GetWorld()->GetGameState<AXivSimGameState>())
	{
		GameState->OnCriticalPatternPhaseChanged.RemoveDynamic(this, &ThisClass::OnCriticalPatternPhaseChanged);
	}
}

void UXivSimGimmickCastingListWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	InitPatternDelegate();
}

void UXivSimGimmickCastingListWidget::InitPatternDelegate()
{
	if (!bInited)
	{
		if (AXivSimGameState* GameState = GetWorld()->GetGameState<AXivSimGameState>())
		{
			GameState->OnCriticalPatternPhaseChanged.AddUniqueDynamic(this, &ThisClass::OnCriticalPatternPhaseChanged);
			bInited = true;
		}
	}
}

void UXivSimGimmickCastingListWidget::OnCriticalPatternPhaseChanged(class UXivSimPatternBase* Pattern,
	EPatternPhase Phase)
{
	if (Phase == EPatternPhase::Phase_Preparing)
	{
		if (const FXivSimPatternData* Data = Pattern->GetPatternData())
		{
			if(CastingBarClass)
			{
				UXivSimGimmickCastingBarWidget* NewBar = CreateWidget<UXivSimGimmickCastingBarWidget>(GetOwningPlayer(), CastingBarClass);
				if(NewBar)
				{
					NewBar->OnCastingFinished.AddUniqueDynamic(this, &ThisClass::OnCastingBarFinished);
					NewBar->SetPatternData(Data);
					CastingList->AddChildToVerticalBox(NewBar);
				}
			}
		}
	}
}

void UXivSimGimmickCastingListWidget::OnCastingBarFinished(UXivSimGimmickCastingBarWidget* BarWidget)
{
	if (BarWidget)
	{
		BarWidget->OnCastingFinished.RemoveDynamic(this, &ThisClass::OnCastingBarFinished);
		CastingList->RemoveChild(BarWidget);
	}
}
