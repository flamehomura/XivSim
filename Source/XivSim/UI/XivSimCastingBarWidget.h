// Copyright@Homura Akane@Chocobo

#pragma once

#include "CoreMinimal.h"
#include "XivSim.h"
#include "XivSimUserWidget.h"
#include "XivSimCastingBarWidget.generated.h"

class UImage;
class UTextBlock;
class UProgressBar;
class AXivSimCharacter;

/**
 * 
 */
UCLASS()
class XIVSIM_API UXivSimCastingBarWidget : public UXivSimUserWidget
{
	GENERATED_UCLASS_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UFUNCTION()
	void ChangePlayerPawn(class APawn* OldPawn, class APawn* NewPawn);
	
	UFUNCTION()
	void OnActionStartCasting(const FXivSimActionData& ActionData);
	UFUNCTION()
	void OnActionCancelCasting(const FXivSimActionData& ActionData);

	void UpdateCastingProgress(float DeltaTime);
	void UpdateFadingOut(float DeltaTime);
	void FadingOut();
	

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> ActionIcon;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ActionName;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> CastingBar;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> InterruptText;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CastingText;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CastingTimeText;

	UPROPERTY()
	TObjectPtr<AXivSimCharacter> CachedPlayerPawn;

	UPROPERTY(EditDefaultsOnly)
	float InterruptedTextBlinkTime = 0.5f;
	
	UPROPERTY(EditDefaultsOnly)
	float FadingOutTime = 0.3f;
	
	bool bIsCasting = false;
	float CurrentCastingTime;
	float MaxCastingTime;
	float CurrentInterruptingTime;
	bool bIsFadingOut = false;
	float CurrentFadingOutTime;
};
