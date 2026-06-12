// Copyright@Homura Akane@chocobo

#pragma once

#include "CoreMinimal.h"
#include "XivSimUserWidget.h"
#include "XivSimTargetBarWidget.generated.h"


class UTextBlock;
class UProgressBar;
class USizeBox;
class AXivSimPlayerState;
class AXivSimCharacterBase;
/**
 * 
 */
UCLASS()
class UXivSimTargetBarWidget : public UXivSimUserWidget
{
	GENERATED_UCLASS_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void OnOwnerPlayerStateChange(class AXivSimPlayerState* PlayerState);

	UFUNCTION()
	void OnTargetCharacterChange(class AXivSimCharacterBase* NewChar);

	UFUNCTION(BlueprintImplementableEvent)
	void SetTargetHostile(bool bHostile);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TargetHealthPct;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TargetName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USizeBox> CastBox;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> CastBar;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CastName;
	
	UPROPERTY(EditAnywhere)
	bool bFocusTargetBarStyle;

	UPROPERTY()
	TObjectPtr<AXivSimPlayerState> CurrentOwnerPlayerState;
	UPROPERTY()
	TObjectPtr<AXivSimCharacterBase> CurrentTargetCharacter;
};
