#include "XivSimHUDWidget.h"
#include "XivSimCharacter.h"
#include "XivSimBuffListWidget.h"
#include "XivSimPlayerController.h"

UXivSimHUDWidget::UXivSimHUDWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UXivSimHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (GetOwningPlayer())
	{
		if (AXivSimCharacter* PlayerPawn = Cast<AXivSimCharacter>(GetOwningPlayer()->GetPawn()))
		{
			ChangePlayerPawn(nullptr, PlayerPawn);
		}
		GetOwningPlayer()->OnPossessedPawnChanged.AddUniqueDynamic(this, &UXivSimHUDWidget::ChangePlayerPawn);
	}
}



void UXivSimHUDWidget::NativeDestruct()
{
	if (GetOwningPlayer())
	{
		GetOwningPlayer()->OnPossessedPawnChanged.RemoveDynamic(this, &UXivSimHUDWidget::ChangePlayerPawn);
	}
	Super::NativeDestruct();
}

void UXivSimHUDWidget::ChangePlayerPawn(class APawn* OldPawn, class APawn* NewPawn)
{
	if (AXivSimCharacter* InXivSimPawn = Cast<AXivSimCharacter>(NewPawn))
	{
		if (OwnerPlayerPawn != InXivSimPawn)
		{
			OwnerPlayerPawn = InXivSimPawn;
			BuffListWidget->SetPlayerPawn(OwnerPlayerPawn);
		}
	}
}