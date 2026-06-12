#pragma once

#include "XivSimBuffPaired.h"
#include "XivSimBuffPatch.generated.h"

UENUM(BlueprintType)
enum class EXivSimBuffPatchType : uint8
{
	PatchType_None,
	PatchType_Far,
	PatchType_Near,
};

UCLASS()
class UXivSimBuffPatch : public UXivSimBuffPaired
{
	GENERATED_BODY()
	
public:
	UXivSimBuffPatch();
	virtual void TickBuff(float DeltaSeconds) override;

	virtual void OnTimeOver() override;
	void TriggerPatch(bool bCritical = false);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	void CheckPatchEffect();
	float GetDistanceToTarget();

protected:
	UPROPERTY(EditDefaultsOnly, Category = PatchEffect)
	EXivSimBuffPatchType PatchType;

	UPROPERTY(EditDefaultsOnly, Category = PatchEffect)
	TSubclassOf<UXivSimBuffPaired> PatchOverBuffClass;

	UPROPERTY(EditDefaultsOnly, Category = PatchEffect)
	bool bMaterialized;

	UPROPERTY(EditDefaultsOnly, Category = PatchEffect)
	float LimitDistance;

	UPROPERTY(EditDefaultsOnly, Category = PatchDamage)
	int32 PatchPatternID;

	UPROPERTY(EditDefaultsOnly, Category = PatchDamage)
	int32 PatchCriticalPatternID;
};