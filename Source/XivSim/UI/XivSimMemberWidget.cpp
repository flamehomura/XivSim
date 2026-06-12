#include "XivSimMemberWidget.h"
#include "XivSimBuffListWidget.h"
#include "XivSimPlayerState.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "XivSimBuffListWidget.h"
#include "XivSimCharacter.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture2D.h"
#include "XivSimMarkerWidget.h"
#include "XivSimPlayerController.h"
#include "Components/Button.h"

UXivSimMemberWidget::UXivSimMemberWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UXivSimMemberWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MarkerImage->HidePlayerName(true);
	MemberButton->OnClicked.AddUniqueDynamic(this, &UXivSimMemberWidget::OnMemberListClicked);
}

void UXivSimMemberWidget::SetPlayerState(class AXivSimPlayerState* PlayerState)
{
	if (MemberPlayerState != PlayerState)
	{
		if (MemberPlayerState)
		{
			MemberPlayerState->OnPawnSet.RemoveDynamic(this, &UXivSimMemberWidget::OnPlayerPawnSet);
			MemberPlayerState->OnPlayerNameChange.RemoveDynamic(this, &UXivSimMemberWidget::OnPlayerNameChanged);
			MemberPlayerState->OnClassTypeChange.RemoveDynamic(this, &UXivSimMemberWidget::OnPlayerClassTypeChanged);
			MemberPlayerState->OnPositionTypeChange.RemoveDynamic(this, &UXivSimMemberWidget::OnPlayerPositionChanged);
		}

		MemberPlayerState = PlayerState;
		MarkerImage->SetPlayerState(PlayerState);

		if (MemberPlayerState)
		{
			UEnum* PosEnum = StaticEnum<EXivSimPositionType>();
			FString PosString = PosEnum->GetDisplayNameTextByValue((int64)MemberPlayerState->GetCurrentPositionType()).ToString();
			FString PlayerName = FString::Printf(TEXT("%s(%s)"), *MemberPlayerState->GetPlayerName(), *PosString);
			PlayerNameText->SetText(FText::FromString(PlayerName));

			OnPlayerPawnSet(MemberPlayerState, MemberPlayerState->GetPawn(), nullptr);
			MemberPlayerState->OnPawnSet.AddUniqueDynamic(this, &UXivSimMemberWidget::OnPlayerPawnSet);
			MemberPlayerState->OnPlayerNameChange.AddUniqueDynamic(this, &UXivSimMemberWidget::OnPlayerNameChanged);
			MemberPlayerState->OnClassTypeChange.AddUniqueDynamic(this, &UXivSimMemberWidget::OnPlayerClassTypeChanged);
			MemberPlayerState->OnPositionTypeChange.AddUniqueDynamic(this, &UXivSimMemberWidget::OnPlayerPositionChanged);
		}
	}
}

void UXivSimMemberWidget::OnPlayerPawnSet(class APlayerState* PlayerState, class APawn* NewPawn, class APawn* OldPawn)
{
	if (MemberPlayerState == PlayerState && MemberPlayerPawn != NewPawn)
	{
		if (MemberPlayerPawn)
		{
			MemberPlayerPawn->OnPlayerHealthChange.RemoveDynamic(this, &UXivSimMemberWidget::OnPlayerHealthChanged);
		}

		MemberPlayerPawn = Cast<AXivSimCharacter>(NewPawn);

		if (MemberPlayerPawn)
		{
			BarrierBar->SetVisibility(ESlateVisibility::HitTestInvisible);
			HealthText->SetVisibility(ESlateVisibility::HitTestInvisible);
			HealthBar->SetVisibility(ESlateVisibility::HitTestInvisible);
			BuffList->SetVisibility(ESlateVisibility::HitTestInvisible);
			OnPlayerHealthChanged(MemberPlayerPawn->GetHealth(), MemberPlayerPawn->GetMaxHealth());

			MemberPlayerPawn->OnPlayerHealthChange.AddUniqueDynamic(this, &UXivSimMemberWidget::OnPlayerHealthChanged);

			if (TObjectPtr<UTexture2D>* ClassTexPtr = ClassTypeTextureMap.Find(MemberPlayerState->GetCurrentClassType()))
			{
				RoleIcon->SetBrushFromTexture(*ClassTexPtr);
			}

			BuffList->SetPlayerPawn(MemberPlayerPawn);
		}
		else
		{
			BarrierBar->SetVisibility(ESlateVisibility::Hidden);
			HealthText->SetVisibility(ESlateVisibility::Hidden);
			HealthBar->SetVisibility(ESlateVisibility::Hidden);
			BuffList->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UXivSimMemberWidget::OnPlayerNameChanged(FString NewName)
{
	if (MemberPlayerState)
	{
		UEnum* PosEnum = StaticEnum<EXivSimPositionType>();
		FString PosString = PosEnum->GetDisplayNameTextByValue((int64)MemberPlayerState->GetCurrentPositionType()).ToString();
		FString PlayerName = FString::Printf(TEXT("%s(%s)"), *NewName, *PosString);
		PlayerNameText->SetText(FText::FromString(PlayerName));
	}
}

void UXivSimMemberWidget::OnPlayerPositionChanged(EXivSimPositionType NewPos)
{
	if (MemberPlayerState)
	{
		UEnum* PosEnum = StaticEnum<EXivSimPositionType>();
		FString PosString = PosEnum->GetDisplayNameTextByValue((int64)NewPos).ToString();
		FString PlayerName = FString::Printf(TEXT("%s(%s)"), *MemberPlayerState->GetPlayerName(), *PosString);
		PlayerNameText->SetText(FText::FromString(PlayerName));
	}
}

void UXivSimMemberWidget::OnPlayerClassTypeChanged(EXivSimClassType NewType)
{
	if (TObjectPtr<UTexture2D>* ClassTexPtr = ClassTypeTextureMap.Find(NewType))
	{
		RoleIcon->SetBrushFromTexture(*ClassTexPtr);
	}
}

void UXivSimMemberWidget::OnPlayerHealthChanged(int32 CurHealth, int32 MaxHeath)
{
	if (MemberPlayerPawn)
	{
		HealthText->SetText(FText::AsNumber(CurHealth));
		float HealthPct = FMath::Clamp((float)CurHealth / (float)MaxHeath, 0.f, 1.f);
		SetPlayerHealthPercent(HealthPct);
	}
}

void UXivSimMemberWidget::SetPlayerHealthPercent(float Value)
{
	if (HealthBar)
	{
		UMaterialInstanceDynamic* MID = HealthBar->GetDynamicMaterial();
		if (MID)
		{
			MID->SetScalarParameterValue(FName("Percent"), Value);
		}
	}
}

void UXivSimMemberWidget::OnMemberListClicked()
{
	if(MemberPlayerPawn)
	{
		if(AXivSimPlayerController* PC = GetOwningXivSimPC())
		{
			PC->TrySelectCharacterInWorld(MemberPlayerPawn);
		}
	}
}
