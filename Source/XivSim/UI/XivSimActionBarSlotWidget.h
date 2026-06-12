// Copyright@Homura Akane@Chocobo

#pragma once

class UImage;
class UPanelWidget;
class UTextBlock;
class AXivSimPlayerState;
class UXivSimHotKeyWidget;
class UInputAction;

#include "CoreMinimal.h"
#include "XivSim.h"
#include "XivSimUserWidget.h"
#include "XivSimActionBarSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class XIVSIM_API UXivSimActionBarSlotWidget : public UXivSimUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	void SetIndex(int32 NewIdx);
	void SetBindInputAction(class UInputAction* NewAction);
	
	void SetEmpty();
	void SetActionData(const FXivSimActionData& InData);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> BGImage;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> ActionBox;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> SlotText;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UXivSimHotKeyWidget> ActionButtonWidgetClass;

	UPROPERTY(SkipSerialization)
	TObjectPtr<UXivSimHotKeyWidget> ActionButtonWidget;
	
	UPROPERTY(SkipSerialization)
	int32 SlotIndex;
	
	UPROPERTY(SkipSerialization)
	TObjectPtr<UInputAction> BindAction;
};
