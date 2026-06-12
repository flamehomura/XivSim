#pragma once

#include "XivSimUserWidget.h"
#include "XivSimHUDWidget.generated.h"

class UXivSimPartyWidget;
class UXivSimBuffListWidget;
class AXivSimCharacter;

UCLASS()
class UXivSimHUDWidget : public UXivSimUserWidget
{
	GENERATED_UCLASS_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void ChangePlayerPawn(class APawn* OldPawn, class APawn* NewPawn);
	
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UXivSimPartyWidget> PartyWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UXivSimBuffListWidget> BuffListWidget;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AXivSimCharacter> OwnerPlayerPawn;
};