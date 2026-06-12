#pragma once

#include "CoreMinimal.h"
#include "XivSim.h"
#include "XivSimManagerBase.generated.h"

UCLASS(BlueprintType, Blueprintable)
class UXivSimManagerBase : public UObject
{
	GENERATED_BODY()
	
public:
	UXivSimManagerBase();

	virtual void Init();
	virtual void UnInit();
	bool HasAuthority();
};
