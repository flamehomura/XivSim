// Copyright@Homura Akane@Chocobo

#pragma once

#include "CoreMinimal.h"
#include "XivSim.h"
#include "XivSimUserWidget.h"
#include "XivSimActionBarWidget.generated.h"

class AXivSimPlayerState;
class UInputAction;
class UHorizontalBox;

/**
 * 
 */
UCLASS()
class UXivSimActionBarWidget : public UXivSimUserWidget
{
	GENERATED_UCLASS_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void OnOwnerPlayerStateChange(class AXivSimPlayerState* PlayerState);
	
	UFUNCTION()
	void OnPlayerClassChanged(EXivSimClassType ClassType);

	void InitActionSlots(const TArray<FXivSimActionData>& InActionDataArray);
	
protected:
	UPROPERTY()
	TObjectPtr<AXivSimPlayerState> OwnerPlayerState;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UInputAction>> InputActionArray;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHorizontalBox> BarBox;
};
