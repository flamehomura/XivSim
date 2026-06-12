#pragma once

#include "CoreMinimal.h"
#include "XivSimBuffBase.generated.h"

class UBuffComponent;
class UTexture2D;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS(BlueprintType, Blueprintable)
class UXivSimBuffBase : public UObject
{
	GENERATED_BODY()

public:
	UXivSimBuffBase();

	virtual void Refresh();
	virtual void OnActivated(bool bClientSimulated = false);
	UFUNCTION(BlueprintImplementableEvent)
	void OnActivatedBP(bool bClientSimulated);
	virtual void OnDeactivated(bool bClientSimulated = false);
	UFUNCTION(BlueprintImplementableEvent)
	void OnDeactivatedBP(bool bClientSimulated);
	virtual void OnTimeOver();
	UFUNCTION(BlueprintCallable)
	virtual void DeactivateSelf();
	virtual void TickBuff(float DeltaSeconds);
	UFUNCTION(BlueprintImplementableEvent)
	void OnTickBP(float DeltaSeconds);

	UFUNCTION(BlueprintCallable)
	void SetMaxTime(float NewTime);
	
	UFUNCTION(BlueprintPure)
	float GetMaxTime() const;

	UFUNCTION(BlueprintPure)
	bool IsActive() const { return bIsActive; }
	bool IsAllowStack() const { return bAllowStack; }
	bool IsAllowOverwrite() const { return bAllowOverwrite; }
	bool IsPenetrateGodMode() const { return bPenetrateGodMode; }
	bool IsAttentionBuff() const { return bAttentionBuff; }
	void AddStack(int32 InStackCount);
	UFUNCTION(BlueprintPure)
	int32 GetStackCount() const { return StackCount; }
	UFUNCTION(BlueprintImplementableEvent)
	void OnStackChangedBP();
	class UTexture2D* GetBuffIcon() const;
	UFUNCTION(BlueprintPure)
	float GetRemainingTime() const;

	UFUNCTION(BlueprintPure)
	AActor* GetOwnerActor();

	void SetOwnerBuffComponent(class UBuffComponent* BuffComp);

	UFUNCTION(BlueprintPure)
	class UBuffComponent* GetOwnerBuffComponent() const { return OwnerBuffComponent; }

	virtual bool IsSupportedForNetworking() const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker);

	UFUNCTION(BlueprintPure)
	class UNiagaraComponent* GetOwnedFXComponent() const;
	
	virtual void ProcessDamageModifier(float& DamageAmount, const TSubclassOf<class UDamageType>& DamageTypeClass);
	UFUNCTION(BlueprintNativeEvent)
	float ProcessDamageModifierBP(float DamageAmount, TSubclassOf<class UDamageType> DamageTypeClass);

protected:
	UFUNCTION(BlueprintPure)
	bool HasAuthority();
	UFUNCTION(BlueprintPure)
	bool IsDedicatedServer();
	UFUNCTION(BlueprintPure)
	class AXivSimCharacter* GetPlayerOwner();

	virtual void CreateFXEffect();
	void TickBuffRemainingTime(float DeltaSeconds);

	UFUNCTION(BlueprintImplementableEvent)
	void OnTimeOver_BP();

	void SetRemainingTime(float NewValue, bool bSetReplicate = false);
	UFUNCTION()
	void OnRep_RemainingTime();
	UFUNCTION()
	void OnRep_StackCount();

	UFUNCTION()
	void OnRep_FXDeactivated();

protected:
	UPROPERTY(Replicated)
	TObjectPtr<UBuffComponent> OwnerBuffComponent;

	UPROPERTY(EditDefaultsOnly, Category = BuffConfig)
	TObjectPtr<UTexture2D> BuffIconTexture;

	UPROPERTY(EditDefaultsOnly, Category = BuffConfig)
	float MaxTime;

	UPROPERTY(EditDefaultsOnly, Category = BuffConfig)
	bool bAllowStack;

	UPROPERTY(EditDefaultsOnly, Category = BuffConfig)
	bool bAllowOverwrite;
	
	UPROPERTY(EditDefaultsOnly, Category = BuffConfig)
	bool bPenetrateGodMode;
	
	UPROPERTY(EditDefaultsOnly, Category = BuffConfig)
	bool bAttentionBuff;

	UPROPERTY(ReplicatedUsing = OnRep_RemainingTime)
	float RemainingTime;
	bool bWantToReplicateRemainingTime;
	float LocalRemainingTime;

	UPROPERTY(ReplicatedUsing = OnRep_StackCount)
	int32 StackCount;

	UPROPERTY(EditDefaultsOnly, Category = BuffEffect)
	TObjectPtr<UNiagaraSystem> FXSystemTemplete;
	TObjectPtr<UNiagaraComponent> FXSystemComponent;

	UPROPERTY(EditDefaultsOnly, Category = BuffConfig)
	float FXDeactiveDelayTime;
	UPROPERTY(ReplicatedUsing = OnRep_FXDeactivated)
	bool bFXDeactivated;
	UPROPERTY(Replicated)
	bool bIsActive;
};