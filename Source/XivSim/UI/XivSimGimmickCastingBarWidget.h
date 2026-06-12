// Copyright@赤音焰@Bilibili

#pragma once

#include "CoreMinimal.h"
#include "XivSimUserWidget.h"
#include "XivSimGimmickCastingBarWidget.generated.h"

class UVerticalBox;
class UTextBlock;
class UProgressBar;
class AXivSimCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGimmickCastingFinished, UXivSimGimmickCastingBarWidget*, BarWidget);
	
/**
 * 
 */
UCLASS()
class XIVSIM_API UXivSimGimmickCastingBarWidget : public UXivSimUserWidget
{
	GENERATED_UCLASS_BODY()
	
public:
	void SetPatternData(const FXivSimPatternData* NewData);
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	void UpdateCastingProgress(float DeltaTime);
	
	UFUNCTION()
	void OnCriticalPatternPhaseChanged(class UXivSimPatternBase* Pattern, EPatternPhase Phase);
	
public:
	UPROPERTY(BlueprintAssignable)
	FGimmickCastingFinished OnCastingFinished;
	
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UVerticalBox> CastingBox;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ActionName;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> CastingBar;
	
	bool bInited = false;
	float CurrentCastingTime = 0.f;
	float MaxCastingTime = 0.f;
};
