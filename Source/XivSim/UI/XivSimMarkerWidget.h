#pragma once

#include "XivSimUserWidget.h"
#include "XivSim.h"
#include "XivSimMarkerWidget.generated.h"

class AXivSimPlayerState;
class UTextBlock;
class UImage;
class UTexture2D;

UCLASS()
class UXivSimMarkerWidget : public UXivSimUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	void SetPlayerState(class AXivSimPlayerState* PlayerState);
	void HidePlayerName(bool bHide);

protected:
	UFUNCTION()
	void OnMarkerTypeChanged(EXivSimMarkerType NewType);

	UFUNCTION()
	void OnPlayerNameChanged(FString NewName);
	
	UFUNCTION()
	void OnPlayerClassChanged(EXivSimClassType ClassType);

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AXivSimPlayerState> OwnerPlayerState;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerNameText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> MarkerIcon;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> ClassIcon;

	UPROPERTY(EditDefaultsOnly)
	TMap<EXivSimMarkerType, TObjectPtr<UTexture2D>> MarkerIconTextureMap;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<EXivSimClassType, TObjectPtr<UTexture2D>> ClassIconTextureMap;
};