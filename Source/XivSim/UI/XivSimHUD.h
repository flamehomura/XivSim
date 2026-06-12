#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "XivSimHUD.generated.h"

class UXivSimHUDWidget;

UCLASS()
class AXivSimHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

public:
	virtual void PostInitializeComponents() override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UXivSimHUDWidget> LayoutWidgetClass;

	UPROPERTY()
	TObjectPtr<UXivSimHUDWidget> LayoutWidget;
};