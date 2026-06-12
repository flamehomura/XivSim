// Copyright@Homura Akane@Chocobo

#pragma once

#include "XivSimBuffPaired.h"
#include "XivSimBuffChain.generated.h"

/**
 * 
 */
UCLASS()
class XIVSIM_API UXivSimBuffChain : public UXivSimBuffPaired
{
	GENERATED_BODY()

public:
	UXivSimBuffChain();
	virtual void TickBuff(float DeltaSeconds) override;
	virtual void OnTimeOver() override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = PatchEffect)
	TSubclassOf<UXivSimBuffPaired> TimeOverBuffClass;
};
