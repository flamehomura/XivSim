// Copyright@Homura Akane@Chocobo


#include "XivSimCastingBarWidget.h"

#include "XivSimActionComponent.h"
#include "XivSimCharacter.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


UXivSimCastingBarWidget::UXivSimCastingBarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentCastingTime = 0.f;
	MaxCastingTime = 0.f;
	CurrentInterruptingTime = 0.f;
	CurrentFadingOutTime = 0.f;
}

void UXivSimCastingBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibility(ESlateVisibility::Collapsed);
	InterruptText->SetVisibility(ESlateVisibility::Collapsed);
	
	if (GetOwningPlayer())
	{
		GetOwningPlayer()->OnPossessedPawnChanged.AddUniqueDynamic(this, &UXivSimCastingBarWidget::ChangePlayerPawn);
		if (AXivSimCharacter* PlayerPawn = Cast<AXivSimCharacter>(GetOwningPlayer()->GetPawn()))
		{
			ChangePlayerPawn(nullptr, PlayerPawn);
		}
	}
}

void UXivSimCastingBarWidget::NativeDestruct()
{
	if (GetOwningPlayer())
	{
		GetOwningPlayer()->OnPossessedPawnChanged.RemoveDynamic(this, &UXivSimCastingBarWidget::ChangePlayerPawn);
	}
	Super::NativeDestruct();
}

void UXivSimCastingBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateFadingOut(InDeltaTime);
	UpdateCastingProgress(InDeltaTime);
}

void UXivSimCastingBarWidget::ChangePlayerPawn(APawn* OldPawn, APawn* NewPawn)
{
	if(CachedPlayerPawn != NewPawn)
	{
		if(CachedPlayerPawn)
		{
			if(UXivSimActionComponent* ActionComponent = CachedPlayerPawn->GetActionComponent())
			{
				ActionComponent->OnActionStartCasting.RemoveDynamic(this, &UXivSimCastingBarWidget::OnActionStartCasting);
				ActionComponent->OnActionCancelCasting.RemoveDynamic(this, &UXivSimCastingBarWidget::OnActionCancelCasting);
			}
		}

		CachedPlayerPawn = Cast<AXivSimCharacter>(NewPawn);

		if(CachedPlayerPawn)
		{
			if(UXivSimActionComponent* ActionComponent = CachedPlayerPawn->GetActionComponent())
			{
				ActionComponent->OnActionStartCasting.AddUniqueDynamic(this, &UXivSimCastingBarWidget::OnActionStartCasting);
				ActionComponent->OnActionCancelCasting.AddUniqueDynamic(this, &UXivSimCastingBarWidget::OnActionCancelCasting);
			}
		}
	}
}

void UXivSimCastingBarWidget::OnActionStartCasting(const FXivSimActionData& ActionData)
{
	if(ActionData.CastingTime > 0.f)
	{
		SetVisibility(ESlateVisibility::HitTestInvisible);
		InterruptText->SetVisibility(ESlateVisibility::Collapsed);

		ActionIcon->SetBrushFromTexture(ActionData.ActionIcon);
		ActionName->SetText(ActionData.ActionName);

		bIsCasting = true;
		MaxCastingTime = ActionData.CastingTime;
		CurrentCastingTime = MaxCastingTime;

		bIsFadingOut = false;
		SetRenderOpacity(1.f);
	}
}

void UXivSimCastingBarWidget::OnActionCancelCasting(const FXivSimActionData& ActionData)
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
	InterruptText->SetVisibility(ESlateVisibility::HitTestInvisible);
	
	bIsCasting = false;
	CurrentInterruptingTime = InterruptedTextBlinkTime;
	
	bIsFadingOut = false;
	SetRenderOpacity(1.f);
}

void UXivSimCastingBarWidget::UpdateCastingProgress(float DeltaTime)
{
	if(bIsCasting)
	{
		if(MaxCastingTime > 0.f)
		{
			CurrentCastingTime = FMath::Max(CurrentCastingTime - DeltaTime, 0.f);
			CastingBar->SetPercent((MaxCastingTime - CurrentCastingTime) / MaxCastingTime);
			
			FString TimeStr = FString::Printf(TEXT("%02d:%02d"), FMath::FloorToInt(CurrentCastingTime),
				FMath::FloorToInt(FMath::Frac(CurrentCastingTime) * 100.f));
			CastingTimeText->SetText(FText::FromString(TimeStr));
			
			if(CurrentCastingTime <= 0.f)
			{
				bIsCasting = false;
				FadingOut();
			}
		}
	}
	else
	{
		if(CurrentInterruptingTime > 0.f)
		{
			CurrentInterruptingTime = FMath::Max(CurrentInterruptingTime - DeltaTime, 0.f);
			if(CurrentInterruptingTime <= 0.f)
			{
				FadingOut();
			}
		}
	}
}

void UXivSimCastingBarWidget::UpdateFadingOut(float DeltaTime)
{
	if(bIsFadingOut && FadingOutTime > 0.f)
	{
		if(CurrentFadingOutTime > 0.f)
		{
			CurrentFadingOutTime = FMath::Max(CurrentFadingOutTime - DeltaTime, 0.f);
			if(CurrentFadingOutTime <= 0.f)
			{
				bIsFadingOut = false;
			}
			SetRenderOpacity(CurrentFadingOutTime / FadingOutTime);
		}
	}
}

void UXivSimCastingBarWidget::FadingOut()
{
	bIsFadingOut = true;
	CurrentFadingOutTime = FadingOutTime;
}
