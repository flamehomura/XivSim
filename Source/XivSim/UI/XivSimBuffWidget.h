#pragma once

#include "XivSimUserWidget.h"
#include "XivSimBuffWidget.generated.h"

class UXivSimBuffBase;
class UImage;
class UTextBlock;

UCLASS()
class UXivSimBuffWidget : public UXivSimUserWidget
{
	GENERATED_UCLASS_BODY()
	
public:
	void SetBuffPtr(class UXivSimBuffBase* InBuffPtr);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void UpdateBuffTime();

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UXivSimBuffBase> Buff;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> BuffIcon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> BuffStack;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> BuffTime;
};