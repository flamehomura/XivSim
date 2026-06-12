// Copyright@Homura Akane@Chocobo

#pragma once

#include "CoreMinimal.h"
#include "XivSimBuffBase.h"
#include "XivSimBuffDamageOverTime.generated.h"

/**
 * 
 */
UCLASS()
class UXivSimBuffDamageOverTime : public UXivSimBuffBase
{
	GENERATED_BODY()
	
public:
	UXivSimBuffDamageOverTime();
	virtual void TickBuff(float DeltaSeconds) override;
	virtual void OnActivated(bool bClientSimulated) override;
	virtual void OnDeactivated(bool bClientSimulated) override;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	float DamageInterval = 3.f;
	
	UPROPERTY(EditDefaultsOnly)
	float DamageAmount;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UXivSimDamageType> DamageTypeClass;

	float DamageTimer;
};
