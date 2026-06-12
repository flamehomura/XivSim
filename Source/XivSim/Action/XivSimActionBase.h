// Copyright@Homura Akane@Chocobo

#pragma once

#include "CoreMinimal.h"
#include "XivSim.h"
#include "UObject/Object.h"
#include "XivSimActionBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UXivSimActionBase : public UObject
{
	GENERATED_BODY()

public:
	void SetActionData(const FXivSimActionData& InData);
	void ActivateAction(class AXivSimCharacter* SourceCharacter, class AXivSimCharacterBase* TargetCharacter);
	void CancelAction(class AXivSimCharacter* SourceCharacter);
	void EndAction(class AXivSimCharacter* SourceCharacter);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void ActivateAction_BP(class AXivSimCharacter* SourceCharacter, class AXivSimCharacterBase* TargetCharacter);

protected:
	UPROPERTY(SkipSerialization)
	FXivSimActionData ActionData;
};
