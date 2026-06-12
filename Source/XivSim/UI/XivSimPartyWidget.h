#pragma once

#include "XivSimUserWidget.h"
#include "XivSim.h"
#include "XivSimPartyWidget.generated.h"

class UPanelWidget;
class AXivSimPlayerState;
class UXivSimMemberWidget;

UCLASS()
class UXivSimPartyWidget : public UXivSimUserWidget
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	UFUNCTION()
	void RebuildMemberWidgets();
	
	UFUNCTION()
	void OnPlayerStateAdd(class AXivSimPlayerState* PlayerState);

	UFUNCTION()
	void OnPlayerStateRemove(class AXivSimPlayerState* PlayerState);

	UFUNCTION()
	void OnOwnerPlayerStateChange(class AXivSimPlayerState* PlayerState);
	
	UFUNCTION()
	void OnPlayerPositionChanged(EXivSimPositionType PosType);

	UFUNCTION(BlueprintCallable)
	void SortPartyList(bool bPositive, bool bLocalFirst);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> PartyPanel;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UXivSimMemberWidget> MemberWidgetClass;

	UPROPERTY()
	TMap<TObjectPtr<AXivSimPlayerState>, TObjectPtr<UXivSimMemberWidget>> MemberWidgetMap;

	FTimerHandle DelayTimerHandle;
};