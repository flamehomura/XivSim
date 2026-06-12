#include "XivSimUserWidget.h"

#include "XivSimCharacter.h"
#include "XivSimPlayerController.h"

UXivSimUserWidget::UXivSimUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

AXivSimPlayerController* UXivSimUserWidget::GetOwningXivSimPC() const
{
	if(GetOwningPlayer())
	{
		return Cast<AXivSimPlayerController>(GetOwningPlayer());
	}

	return nullptr;
}

AXivSimCharacter* UXivSimUserWidget::GetOwningXivSimPlayer() const
{
	if(AXivSimPlayerController* PC = GetOwningXivSimPC())
	{
		return Cast<AXivSimCharacter>(PC->GetPawn());
	}

	return nullptr;
}
