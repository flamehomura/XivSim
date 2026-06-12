// Copyright@Homura Akane@Chocobo

#pragma once

#include "CoreMinimal.h"
#include "XivSimBuffPaired.h"
#include "XivSimBuffBind.generated.h"

/**
 * 
 */
UCLASS()
class XIVSIM_API UXivSimBuffBind : public UXivSimBuffPaired
{
	GENERATED_BODY()
	
public:
	UXivSimBuffBind();
	virtual void TickBuff(float DeltaSeconds) override;
	virtual void SetPairedBuff(class UXivSimBuffPaired* Buff) override;

protected:
	void CheckBindDistance();
	float GetDistanceToTarget();
	void TriggerPattern();
	
protected:
	float InitialDistance;
	
	UPROPERTY(EditDefaultsOnly, Category = BindEffect)
	float MinDistance = 2000.f;
	
	UPROPERTY(EditDefaultsOnly, Category = BindEffect)
	float MaxDistance = 4000.f;
	
	UPROPERTY(EditDefaultsOnly, Category = BindDamage)
	int32 CriticalPatternID;
};
