// Copyright@赤音焰@Bilibili


#include "XivSimGimmickCastingBarWidget.h"

#include "../Manager/XivSimPatternManager.h"
#include "../Pattern/XivSimPatternBase.h"
#include "../Gameplay/XivSimGameState.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

UXivSimGimmickCastingBarWidget::UXivSimGimmickCastingBarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentCastingTime = 0.f;
	MaxCastingTime = 0.f;
}

void UXivSimGimmickCastingBarWidget::SetPatternData(const FXivSimPatternData* NewData)
{
	if (NewData)
	{
		ActionName->SetText(NewData->PatternName);
		MaxCastingTime = NewData->PatternRangeDelay;
		CurrentCastingTime = 0.f;
		if (MaxCastingTime > 0.f)
		{
			CastingBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			OnCastingFinished.Broadcast(this);
		}
	}
}

void UXivSimGimmickCastingBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UXivSimGimmickCastingBarWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UXivSimGimmickCastingBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateCastingProgress(InDeltaTime);
}

void UXivSimGimmickCastingBarWidget::UpdateCastingProgress(float DeltaTime)
{
	if (CurrentCastingTime <= MaxCastingTime)
	{
		CurrentCastingTime = FMath::Max(CurrentCastingTime + DeltaTime, 0.f);
		CastingBar->SetPercent(CurrentCastingTime / MaxCastingTime);
	}
	if (CurrentCastingTime > MaxCastingTime)
	{
		OnCastingFinished.Broadcast(this);
	}
}

void UXivSimGimmickCastingBarWidget::OnCriticalPatternPhaseChanged(class UXivSimPatternBase* Pattern,
	EPatternPhase Phase)
{
	if (Phase == EPatternPhase::Phase_Preparing)
	{
		if (const FXivSimPatternData* Data = Pattern->GetPatternData())
		{
			ActionName->SetText(Data->PatternName);
			MaxCastingTime = Data->PatternRangeDelay;
			CurrentCastingTime = 0.f;
			if (MaxCastingTime > 0.f)
			{
				CastingBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}
	}
}