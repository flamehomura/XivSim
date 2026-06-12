#include "XivSimMarkerWidget.h"
#include "XivSimPlayerState.h"
#include "Engine/Texture2D.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

UXivSimMarkerWidget::UXivSimMarkerWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UXivSimMarkerWidget::SetPlayerState(class AXivSimPlayerState* PlayerState)
{
	if (OwnerPlayerState != PlayerState)
	{
		if (OwnerPlayerState)
		{
			OwnerPlayerState->OnMarkerTypeChange.RemoveDynamic(this, &UXivSimMarkerWidget::OnMarkerTypeChanged);
			OwnerPlayerState->OnPlayerNameChange.RemoveDynamic(this, &UXivSimMarkerWidget::OnPlayerNameChanged);
			OwnerPlayerState->OnClassTypeChange.RemoveDynamic(this, &UXivSimMarkerWidget::OnPlayerClassChanged);
		}

		OwnerPlayerState = PlayerState;

		if (OwnerPlayerState)
		{
			OnMarkerTypeChanged(OwnerPlayerState->GetCurrentMarkerType());
			OnPlayerNameChanged(OwnerPlayerState->GetPlayerName());
			OnPlayerClassChanged(OwnerPlayerState->GetCurrentClassType());
			OwnerPlayerState->OnMarkerTypeChange.AddUniqueDynamic(this, &UXivSimMarkerWidget::OnMarkerTypeChanged);
			OwnerPlayerState->OnPlayerNameChange.AddUniqueDynamic(this, &UXivSimMarkerWidget::OnPlayerNameChanged);
			OwnerPlayerState->OnClassTypeChange.AddUniqueDynamic(this, &UXivSimMarkerWidget::OnPlayerClassChanged);
		}
	}
}

void UXivSimMarkerWidget::HidePlayerName(bool bHide)
{
	PlayerNameText->SetVisibility(bHide ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
	ClassIcon->SetVisibility(bHide ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
}

void UXivSimMarkerWidget::OnMarkerTypeChanged(EXivSimMarkerType NewType)
{
	if (MarkerIcon)
	{
		if (TObjectPtr<UTexture2D>* TexPtr = MarkerIconTextureMap.Find(NewType))
		{
			MarkerIcon->SetBrushFromTexture(*TexPtr);
			MarkerIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			MarkerIcon->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UXivSimMarkerWidget::OnPlayerNameChanged(FString NewName)
{
	if (OwnerPlayerState)
	{
		PlayerNameText->SetText(FText::FromString(NewName));
	}
}

void UXivSimMarkerWidget::OnPlayerClassChanged(EXivSimClassType ClassType)
{
	if(ClassIcon)
	{
		if(TObjectPtr<UTexture2D>* TexPtr = ClassIconTextureMap.Find(ClassType))
		{
			ClassIcon->SetBrushFromTexture(*TexPtr);
		}
	}
}
