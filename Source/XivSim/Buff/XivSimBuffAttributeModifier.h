#pragma once

#include "XivSimBuffBase.h"
#include "XivSim.h"
#include "XivSimBuffAttributeModifier.generated.h"

UCLASS()
class UXivSimBuffAttributeModifier : public UXivSimBuffBase
{
	GENERATED_BODY()

public:
	UXivSimBuffAttributeModifier();
	virtual void OnActivated(bool bClientSimulated) override;
	virtual void OnDeactivated(bool bClientSimulated) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	EXivSimAttributeType AttributeType;

	UPROPERTY(EditDefaultsOnly)
	bool bAllowSimulate;

	UPROPERTY(EditDefaultsOnly)
	float ModifierValue;
};