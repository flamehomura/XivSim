// Copyright@Homura Akane@Chocobo

#pragma once

#include "CoreMinimal.h"
#include "XivSim.h"
#include "XivSimUserWidget.h"
#include "XivSimHotKeyWidget.generated.h"

class UXivSimHotKeyButtonWidget;
class UImage;
/**
 * 
 */
UCLASS()
class UXivSimHotKeyWidget : public UXivSimUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetActionDataByID(const int32& NewID);
	void SetActionData(const FXivSimActionData& ActionData);
	void BindAction(class UInputAction* NewAction);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void CheckActionAvailable();
	class UXivSimActionComponent* GetActionComponent();

	UFUNCTION(BlueprintPure)
	bool ActionInCoolingDown();
	
	UFUNCTION(BlueprintPure)
	bool ActionIsCasting();
	
	UFUNCTION(BlueprintPure)
	float ActionCoolingDownTimePercent();
	
	UFUNCTION()
	void OnHotKeyPressed();

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UXivSimHotKeyButtonWidget> HotKey;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> CoolDownImage;

	FXivSimActionData HotKeyActionData;
};
