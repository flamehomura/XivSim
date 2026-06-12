// Copyright@Homura Akane@chocobo


#include "XivSimTargetBarWidget.h"

#include "XivSimCharacter.h"
#include "XivSimCharacterBase.h"
#include "XivSimPlayerController.h"
#include "XivSimPlayerState.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

UXivSimTargetBarWidget::UXivSimTargetBarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UXivSimTargetBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
	CastBox->SetVisibility(ESlateVisibility::Collapsed);
	
	if(AXivSimPlayerController* PC = GetOwningXivSimPC())
	{
		PC->OnPlayerStateChange.AddUniqueDynamic(this, &UXivSimTargetBarWidget::OnOwnerPlayerStateChange);
		if(AXivSimPlayerState* PS = PC->GetPlayerState<AXivSimPlayerState>())
		{
			OnOwnerPlayerStateChange(PS);
		}
	}
}

void UXivSimTargetBarWidget::NativeDestruct()
{
	if (AXivSimPlayerController* PC = Cast<AXivSimPlayerController>(GetOwningPlayer()))
	{
		PC->OnPlayerStateChange.RemoveDynamic(this, &UXivSimTargetBarWidget::OnOwnerPlayerStateChange);
	}
	
	Super::NativeDestruct();
}

void UXivSimTargetBarWidget::OnOwnerPlayerStateChange(AXivSimPlayerState* PlayerState)
{
	if(CurrentOwnerPlayerState != PlayerState)
	{
		if(CurrentOwnerPlayerState)
		{
			CurrentOwnerPlayerState->OnSelectedCharacterChange.RemoveDynamic(this, &UXivSimTargetBarWidget::OnTargetCharacterChange);
		}
		CurrentOwnerPlayerState = PlayerState;
		
		if(CurrentOwnerPlayerState)
		{
			CurrentOwnerPlayerState->OnSelectedCharacterChange.AddUniqueDynamic(this, &UXivSimTargetBarWidget::OnTargetCharacterChange);
		}
	}
}

void UXivSimTargetBarWidget::OnTargetCharacterChange(AXivSimCharacterBase* NewChar)
{
	if(IsValid(NewChar))
	{
		if(TargetName)
		{
			TargetName->SetText(FText::FromString(NewChar->GetCharacterName()));
		}

		if(AXivSimCharacter* OwnerPlayer = GetOwningXivSimPlayer())
		{
			SetTargetHostile(FGenericTeamId::GetAttitude(OwnerPlayer, NewChar) == ETeamAttitude::Type::Hostile);
		}
	}
	
	SetVisibility(IsValid(NewChar) ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}
