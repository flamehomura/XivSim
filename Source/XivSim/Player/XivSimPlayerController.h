#pragma once

#include "XivSim.h"
#include "GameFramework/PlayerController.h"
#include "XivSimPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UMaterialInterface;
class UDecalComponent;
class AXivSimCharacterBase;

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FPlayerStateChangeSignature, AXivSimPlayerController, OnPlayerStateChange, class AXivSimPlayerState*, NewPS);

UCLASS()
class AXivSimPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/** Default Constructor */
	AXivSimPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void ServerExecRPC_Implementation(const FString& Msg) override;

	UFUNCTION(BlueprintCallable)
	void TrySetPlayerName(FString NewName);

	UFUNCTION(BlueprintCallable)
	void TrySetClassType(EXivSimClassType NewType);
	
	UFUNCTION(BlueprintCallable)
	void TrySetPositionType(EXivSimPositionType NewType);

	virtual bool InputKey(const FInputKeyEventArgs& Params) override;

	bool TrySelectWorldActor();
	bool TrySelectCharacterInWorld(class AXivSimCharacterBase* NewCharacter);
	void SelectCharacterInWorld(class AXivSimCharacterBase* NewCharacter);
	UFUNCTION(BlueprintPure)
	class AXivSimCharacterBase* GetCurrentSelectedCharacter() const;

	UFUNCTION(BlueprintCallable)
	void TrySetPartyGodMode(bool bGod);
	UFUNCTION(Server, Reliable)
	void ServerSetPartyGodMode(bool bGod);
	UFUNCTION(BlueprintCallable)
	void TrySetTrainingMode(bool bGod);
	UFUNCTION(Server, Reliable)
	void ServerSetTrainingMode(bool bGod);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupInputComponent() override;

	void TabTargeting();

	virtual void OnRep_Pawn() override;
	virtual void OnRep_PlayerState() override;
	void CreateTargetRingDecal();
	void AttachTargetRingTo(class AXivSimCharacterBase* TargetCharacter);

public:
	UPROPERTY()
	FPlayerStateChangeSignature OnPlayerStateChange;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> TabTargetingAction;
	
	UPROPERTY(EditDefaultsOnly)
	float WorldActorTraceLength = 5000.f;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> TargetRingDecalMaterial;

	UPROPERTY(SkipSerialization)
	bool bSelectionInterrupted;
	
	UPROPERTY(SkipSerialization)
	TObjectPtr<UDecalComponent> TargetRingDecalComponent;
	
	UPROPERTY(SkipSerialization)
	TObjectPtr<AXivSimCharacterBase> CurrentSelectedCharacter;
	
	UPROPERTY(EditDefaultsOnly)
	bool bDisableSelectHitNULL = false;

	float LastSelectionHitTime = 0.f;
	UPROPERTY(EditDefaultsOnly)
	float SelectionInterruptTimeThreshold = 0.1f;

	UPROPERTY(SkipSerialization)
	TArray<TObjectPtr<AXivSimCharacterBase>> CachedTabTargetingList;
	UPROPERTY(SkipSerialization)
	TArray<TObjectPtr<AXivSimCharacterBase>> TempCachedTabTargetingList;
};