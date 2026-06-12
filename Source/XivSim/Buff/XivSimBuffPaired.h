#pragma once
#include "XivSimBuffBase.h"
#include "XivSimBuffPaired.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class UXivSimBuffPaired : public UXivSimBuffBase
{
	GENERATED_BODY()

public:
	UXivSimBuffPaired();
	virtual void OnDeactivated(bool bClientSimulated) override;
	virtual void TickBuff(float DeltaSeconds);

	UFUNCTION(BlueprintCallable)
	virtual void SetPairedBuff(class UXivSimBuffPaired* Buff);
	UFUNCTION(BlueprintCallable)
	void SetIsLeader(bool bLeader) { bIsLeader = bLeader; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void CreateFXEffect() override;
	virtual void CreateChainFXEffect();
	virtual void UpdateFXEffectTarget();
	class UBuffComponent* GetPairedBuffComponent();

protected:
	UPROPERTY(Replicated)
	bool bIsLeader;

	UPROPERTY(Replicated)
	TObjectPtr<UXivSimBuffPaired> PairedBuff;

	UPROPERTY(EditDefaultsOnly, Category = BuffEffect)
	TObjectPtr<UNiagaraSystem> ChainFXSystemTemplete;
	TObjectPtr<UNiagaraComponent> ChainFXSystemComponent;
};