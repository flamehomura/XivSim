#pragma once

#include "XivSimBuffPaired.h"
#include "XivSimBuffEcho.generated.h"

UENUM(BlueprintType)
enum class EXivSimBuffEchoType : uint8
{
	EchoType_Far,
	EchoType_Mid,
	EchoType_Near,
};

UCLASS()
class UXivSimBuffEcho : public UXivSimBuffPaired
{
	GENERATED_BODY()

public:
	UXivSimBuffEcho();
	virtual void OnDeactivated(bool bClientSimulated) override;
	virtual void TickBuff(float DeltaSeconds) override;
	void TriggerPunish(bool bAdd);

protected:
	void CheckEchoEffect();
	float GetDistanceToTarget();

protected:
	UPROPERTY(EditDefaultsOnly, Category = PatchEffect)
	EXivSimBuffEchoType EchoType;

	UPROPERTY(EditDefaultsOnly, Category = PatchEffect)
	TSubclassOf<UXivSimBuffBase> PunishBuffClass;

	UPROPERTY(EditDefaultsOnly, Category = PatchEffect)
	float MaxLimitDistance;

	UPROPERTY(EditDefaultsOnly, Category = PatchEffect)
	float MinLimitDistance;

	UPROPERTY()
	TObjectPtr<UXivSimBuffBase> OwnerPunishBuff;

	UPROPERTY()
	TObjectPtr<UXivSimBuffBase> PairedPunishBuff;
};