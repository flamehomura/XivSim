#pragma once

#include "CoreMinimal.h"
#include "XivSim.h"
#include "XivSimBuffBase.h"
#include "GameFramework\DamageType.h"
#include "XivSimDamageType.generated.h"

UCLASS()
class UXivSimDamageType : public UDamageType
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	EXivSimDamageEffectType DamageEffectType;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UXivSimBuffBase> DamageBuffClass;
};