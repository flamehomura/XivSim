#include "XivSimBuffWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "XivSimBuffBase.h"

UXivSimBuffWidget::UXivSimBuffWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UXivSimBuffWidget::SetBuffPtr(class UXivSimBuffBase* InBuffPtr)
{
	if (Buff != InBuffPtr)
	{
		Buff = InBuffPtr;

		if (Buff)
		{
			BuffIcon->SetBrushFromTexture(Buff->GetBuffIcon());
			BuffStack->SetVisibility(Buff->IsAllowStack() ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
			if (Buff->IsAllowStack())
			{
				BuffStack->SetText(FText::AsNumber(Buff->GetStackCount(), &FNumberFormattingOptions::DefaultNoGrouping()));
			}
			UpdateBuffTime();
		}
	}
}

void UXivSimBuffWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (Buff)
	{
		UpdateBuffTime();
		if (Buff->IsAllowStack())
		{
			BuffStack->SetText(FText::AsNumber(Buff->GetStackCount(), &FNumberFormattingOptions::DefaultNoGrouping()));
		}
	}
}

void UXivSimBuffWidget::UpdateBuffTime()
{
	if (Buff)
	{
		// FNumberFormattingOptions NumberFormatOptions;
		// NumberFormatOptions.MaximumFractionalDigits = 1.f;
		int32 BuffTimeInteger = FMath::RoundToInt(Buff->GetRemainingTime());
		BuffTime->SetVisibility(BuffTimeInteger > 0 ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
		if(BuffTimeInteger >= 3600)
		{
			BuffTime->SetText(FText::FromString(FString::Printf(TEXT("%dh"), BuffTimeInteger / 3600)));
		}
		else if(BuffTimeInteger >= 60)
		{
			BuffTime->SetText(FText::FromString(FString::Printf(TEXT("%dm"), BuffTimeInteger / 60)));
		}
		else
		{
			BuffTime->SetText(FText::AsNumber(BuffTimeInteger));
		}
	}
}
