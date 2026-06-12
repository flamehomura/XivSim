#include "XivSimPlayerMovementComponent.h"
#include "XivSimCharacter.h"

void FSavedMove_XivSimCharacter::Clear()
{
	FSavedMove_Character::Clear();

	bLeftHold = false;
	bRightHold = false;
}

uint8 FSavedMove_XivSimCharacter::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();

	if (bLeftHold)
	{
		Result |= FLAG_Custom_0;
	}
	
	if (bRightHold)
	{
		Result |= FLAG_Custom_1;
	}

	return Result;
}

bool FSavedMove_XivSimCharacter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	if (bLeftHold != ((FSavedMove_XivSimCharacter*)&NewMove)->bLeftHold)
	{
		return false;
	}
	if (bRightHold != ((FSavedMove_XivSimCharacter*)&NewMove)->bRightHold)
	{
		return false;
	}
	return FSavedMove_Character::CanCombineWith(NewMove, Character, MaxDelta);
}

void FSavedMove_XivSimCharacter::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	AXivSimCharacter* XivSimChar = Cast<AXivSimCharacter>(Character);
	if (XivSimChar)
	{
		bLeftHold = XivSimChar->IsMouseLeftHold();
		bRightHold = XivSimChar->IsMouseRightHold();
	}
}

void FSavedMove_XivSimCharacter::PrepMoveFor(ACharacter* Character)
{
	FSavedMove_Character::PrepMoveFor(Character);

	AXivSimCharacter* XivSimChar = Cast<AXivSimCharacter>(Character);
	if (XivSimChar)
	{
		XivSimChar->SetMouseLeftHold(bLeftHold);
		XivSimChar->SetMouseRightHold(bRightHold);
	}
}

FNetworkPredictionData_Client_XivSim::FNetworkPredictionData_Client_XivSim(const UXivSimPlayerMovementComponent& ServerMovement)
	: FNetworkPredictionData_Client_Character(ServerMovement)
{

}

FSavedMovePtr FNetworkPredictionData_Client_XivSim::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_XivSimCharacter());
}

UXivSimPlayerMovementComponent::UXivSimPlayerMovementComponent()
{
}

float UXivSimPlayerMovementComponent::GetMaxSpeed() const
{
	float ResMaxSpeed = Super::GetMaxSpeed();
	if (AXivSimCharacter* XivSimChar = Cast<AXivSimCharacter>(CharacterOwner))
	{
		ResMaxSpeed *= XivSimChar->GetAttributeModifier(EXivSimAttributeType::Attribute_MoveSpeed);
		if (XivSimChar->IsStandardInputMode() && Velocity.GetSafeNormal2D().Dot(UpdatedComponent->GetComponentQuat().GetForwardVector()) < -0.1f)
		{
			ResMaxSpeed *= 0.5f;
		}
	}

	return ResMaxSpeed;
}

void UXivSimPlayerMovementComponent::SetForceMove(FVector ForceVelocity)
{
	if (CharacterOwner->GetLocalRole() > ROLE_SimulatedProxy)
	{
		if(CharacterOwner->GetLocalRole() == ROLE_AutonomousProxy)
		{
			FlushServerMoves();
			FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
			if(ClientData)
			{
				for (int32 i=0; i < ClientData->SavedMoves.Num(); i++)
				{
					ClientData->FreeMove(ClientData->SavedMoves[i]);
				}
				ClientData->SavedMoves.Reset();
			}
		}
		ForceMoveVelocity = ForceVelocity;
	}
}

void UXivSimPlayerMovementComponent::ClearForceMove()
{
	ForceMoveVelocity = FVector::ZeroVector;
}

void UXivSimPlayerMovementComponent::PhysicsRotation(float DeltaTime)
{
	if(AXivSimCharacter* XivSimChar = Cast<AXivSimCharacter>(CharacterOwner))
	{
		if(XivSimChar->IsStunning())
		{
			return;
		}
	}
	
	Super::PhysicsRotation(DeltaTime);
}

void UXivSimPlayerMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	if (!CharacterOwner)
	{
		return;
	}

	Super::UpdateFromCompressedFlags(Flags);

	if(AXivSimCharacter* XivSimChar = Cast<AXivSimCharacter>(CharacterOwner))
	{
		bool bLeftHold = ((Flags & FSavedMove_Character::FLAG_Custom_0) != 0);
		bool bRightHold = ((Flags & FSavedMove_Character::FLAG_Custom_1) != 0);
		if (CharacterOwner->GetLocalRole() == ROLE_Authority)
		{
			XivSimChar->SetMouseLeftHold(bLeftHold);
			XivSimChar->SetMouseRightHold(bRightHold);
		}
	}
}

class FNetworkPredictionData_Client* UXivSimPlayerMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UXivSimPlayerMovementComponent* MutableThis = const_cast<UXivSimPlayerMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_XivSim(*this);
	}

	return ClientPredictionData;
}

void UXivSimPlayerMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)
{
	if (!ForceMoveVelocity.IsNearlyZero())
	{
		Velocity = ForceMoveVelocity;
	}
	else
	{
		Super::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);
	}
}

void UXivSimPlayerMovementComponent::ApplyRootMotionToVelocity(float deltaTime)
{
	if (!ForceMoveVelocity.IsNearlyZero())
	{
		Velocity = ForceMoveVelocity;
	}
	else
	{
		Super::ApplyRootMotionToVelocity(deltaTime);
	}
}
