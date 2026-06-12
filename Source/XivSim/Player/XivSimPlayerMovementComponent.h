#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "XivSimPlayerMovementComponent.generated.h"

class UXivSimPlayerMovementComponent;

class FSavedMove_XivSimCharacter : public FSavedMove_Character
{
public:
	bool bLeftHold = false;
	bool bRightHold = false;

	virtual void Clear() override;
	virtual uint8 GetCompressedFlags() const override;
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
	virtual void PrepMoveFor(class ACharacter* Character) override;
};

class FNetworkPredictionData_Client_XivSim : public FNetworkPredictionData_Client_Character
{
public:
	FNetworkPredictionData_Client_XivSim(const UXivSimPlayerMovementComponent& ServerMovement);
	virtual FSavedMovePtr AllocateNewMove() override;
};

UCLASS()
class UXivSimPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UXivSimPlayerMovementComponent();

	virtual float GetMaxSpeed() const override;
	void SetForceMove(FVector ForceVelocity);
	void ClearForceMove();
	
	virtual void PhysicsRotation(float DeltaTime) override;

protected:
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;
	virtual void ApplyRootMotionToVelocity(float deltaTime) override;

	UPROPERTY()
	FVector ForceMoveVelocity;
};
