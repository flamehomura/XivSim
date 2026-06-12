#pragma once

#include "XivSimBuffBase.h"
#include "XivSim.h"
#include "XivSimBuffDamageModifier.generated.h"

UCLASS()
class UXivSimBuffDamageModifier : public UXivSimBuffBase
{
	GENERATED_BODY()

public:
	UXivSimBuffDamageModifier();
	virtual void OnActivated(bool bClientSimulated) override;
	virtual void OnDeactivated(bool bClientSimulated) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	EXivSimDamageEffectType DamageType;

	UPROPERTY(EditDefaultsOnly)
	float ModifierValue;
};