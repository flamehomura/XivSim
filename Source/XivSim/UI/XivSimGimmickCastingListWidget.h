// Copyright@赤音焰@Bilibili

#pragma once

#include "CoreMinimal.h"
#include "XivSimUserWidget.h"
#include "XivSimGimmickCastingListWidget.generated.h"

class UXivSimGimmickCastingBarWidget;
class UVerticalBox;
/**
 * 
 */
UCLASS()
class XIVSIM_API UXivSimGimmickCastingListWidget : public UXivSimUserWidget
{
	GENERATED_UCLASS_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void InitPatternDelegate();
	
	UFUNCTION()
	void OnCriticalPatternPhaseChanged(class UXivSimPatternBase* Pattern, EPatternPhase Phase);
	
	UFUNCTION()
	void OnCastingBarFinished(UXivSimGimmickCastingBarWidget* BarWidget);
	
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UVerticalBox> CastingList;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UXivSimGimmickCastingBarWidget> CastingBarClass;
	
	bool bInited = false;
};
