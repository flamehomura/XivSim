#include "XivSimHUD.h"
#include "XivSimHUDWidget.h"

AXivSimHUD::AXivSimHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AXivSimHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (LayoutWidgetClass)
	{
		LayoutWidget = CreateWidget<UXivSimHUDWidget>(PlayerOwner, LayoutWidgetClass);
		if (LayoutWidget)
		{
			LayoutWidget->AddToViewport();
		}
	}
}
