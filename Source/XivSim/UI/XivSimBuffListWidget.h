#pragma once

#include "XivSimUserWidget.h"
#include "XivSimBuffListWidget.generated.h"

class AXivSimCharacter;
class UPanelWidget;
class UXivSimBuffWidget;
class UXivSimBuffBase;

UCLASS()
class UXivSimBuffListWidget : public UXivSimUserWidget
{
	GENERATED_UCLASS_BODY()
	
public:
	void SetPlayerPawn(class AXivSimCharacter* NewPawn);

	UFUNCTION()
	void OnBuffAdd(class UXivSimBuffBase* InBuff);

	UFUNCTION()
	void OnBuffRemove(class UXivSimBuffBase* InBuff);

protected:
	UPROPERTY()
	TObjectPtr<AXivSimCharacter> PlayerPawn;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> ListPanel;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UXivSimBuffWidget> BuffWidgetClass;

	UPROPERTY()
	TMap<TObjectPtr<UXivSimBuffBase>, TObjectPtr<UXivSimBuffWidget>> BuffWidgetMap;
};