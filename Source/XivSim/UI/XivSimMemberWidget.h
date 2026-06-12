#pragma once

#include "XivSimUserWidget.h"
#include "XivSim.h"
#include "XivSimMemberWidget.generated.h"

class AXivSimPlayerState;
class AXivSimCharacter;
class UTexture2D;
class UImage;
class UTextBlock;
class UXivSimBuffListWidget;
class UXivSimMarkerWidget;
class UButton;

UCLASS()
class UXivSimMemberWidget : public UXivSimUserWidget
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void NativeConstruct() override;
	void SetPlayerState(class AXivSimPlayerState* PlayerState);

protected:
	UFUNCTION()
	void OnPlayerPawnSet(class APlayerState* PlayerState, class APawn* NewPawn, class APawn* OldPawn);

	UFUNCTION()
	void OnPlayerNameChanged(FString NewName);
	
	UFUNCTION()
	void OnPlayerPositionChanged(EXivSimPositionType NewPos);
	
	UFUNCTION()
	void OnPlayerClassTypeChanged(EXivSimClassType NewType);

	UFUNCTION()
	void OnPlayerHealthChanged(int32 CurHealth, int32 MaxHeath);
	void SetPlayerHealthPercent(float Value);
	
	UFUNCTION()
	void OnMemberListClicked();

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AXivSimPlayerState> MemberPlayerState;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AXivSimCharacter> MemberPlayerPawn;

	UPROPERTY(EditDefaultsOnly)
	TMap<EXivSimClassType, TObjectPtr<UTexture2D>> ClassTypeTextureMap;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> RoleIcon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerNameText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> HealthBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> BarrierBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UXivSimBuffListWidget> BuffList;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UXivSimMarkerWidget> MarkerImage;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> MemberButton;
};