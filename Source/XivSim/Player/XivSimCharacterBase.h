#pragma once

#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "XivSimCharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_TwoParams(FOnCanBeTargetedStateChangedSignature, AXivSimCharacterBase, OnCanBeTargetedStateChanged, AXivSimCharacterBase*, TargetCharacter, bool, bNewCan);

UCLASS()
class AXivSimCharacterBase : public ACharacter, public IGenericTeamAgentInterface
{
public:
	GENERATED_UCLASS_BODY()
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	UFUNCTION(BlueprintPure)
	virtual FGenericTeamId GetGenericTeamId() const override;
	
	UFUNCTION(BlueprintPure)
	virtual FString GetCharacterName() const;

	UFUNCTION(BlueprintPure)
	virtual float GetCharacterHitRadius() const;
	
	UFUNCTION(BlueprintCallable)
	virtual void SetCanBeTargeted(bool bNewValue);
	
	UFUNCTION(BlueprintCallable)
	virtual void SetEntranced(bool bNewValue);

	const bool CanBeTargeted() const;
	void CharacterSelected();
	void CharacterUnselected();

	virtual FVector GetTargetRingRelativeLocation() const;
	virtual FVector GetTargetRingDecalSize() const;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_Entranced();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnEntrancedStateChanged();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnCharacterSelected();
	UFUNCTION(BlueprintImplementableEvent)
	void OnCharacterUnselected();

public:
	UPROPERTY()
	FOnCanBeTargetedStateChangedSignature OnCanBeTargetedStateChanged;

protected:
	UPROPERTY(EditDefaultsOnly, Replicated)
	FGenericTeamId TeamID;
	
	UPROPERTY(EditDefaultsOnly, Replicated)
	FString CharacterName;
	
	UPROPERTY(EditDefaultsOnly)
	float CharacterHitRadius = 100.f;
	
	UPROPERTY(EditDefaultsOnly, Replicated)
	bool bCanBeTargeted = false;

	UPROPERTY(ReplicatedUsing=OnRep_Entranced, BlueprintReadWrite)
	bool bEntranced = true;

	UPROPERTY(BlueprintReadOnly)
	bool bLocalSelected = false;
};
