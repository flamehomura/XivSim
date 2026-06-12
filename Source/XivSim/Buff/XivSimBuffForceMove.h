#pragma once

#include "XivSimBuffBase.h"
#include "XivSimBuffForceMove.generated.h"

UCLASS()
class UXivSimBuffForceMove : public UXivSimBuffBase
{
	GENERATED_BODY()

public:
	UXivSimBuffForceMove();
	virtual void OnActivated(bool bClientSimulated) override;
	virtual void OnDeactivated(bool bClientSimulated) override;

	UFUNCTION(BlueprintCallable)
	void SetForceMoveDirection(FVector NewDir);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UFUNCTION()
	void EnableForceMove();

	UFUNCTION()
	void ClearForceMove();

	UFUNCTION()
	void OnRep_ForceMoveDirection();

	class UXivSimPlayerMovementComponent* GetMovementComponent();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_ForceMoveDirection)
	FVector ForceMoveDirection;

	UPROPERTY(EditAnywhere)
	float ForceMoveSpeed;

	UPROPERTY(EditDefaultsOnly)
	bool bOwnerPivot;
	
	UPROPERTY(EditDefaultsOnly)
	bool bIgnoreAntiBuff;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UXivSimBuffBase> AntiBuffClass;

	bool bIsForceMoveEnabled;
};