
#include "XivSimBuffForceMove.h"
#include "TimerManager.h"
#include "BuffComponent.h"
#include "XivSimCharacter.h"
#include "Net/UnrealNetwork.h"
#include "XivSimPlayerMovementComponent.h"

UXivSimBuffForceMove::UXivSimBuffForceMove()
{
	bIsForceMoveEnabled = false;
	bIgnoreAntiBuff = false;
}

void UXivSimBuffForceMove::OnActivated(bool bClientSimulated)
{
	Super::OnActivated(bClientSimulated);
}

void UXivSimBuffForceMove::OnDeactivated(bool bClientSimulated)
{
	Super::OnDeactivated(bClientSimulated);
	ClearForceMove();
}

void UXivSimBuffForceMove::SetForceMoveDirection(FVector NewDir)
{
	ForceMoveDirection = NewDir;
	EnableForceMove();
}

void UXivSimBuffForceMove::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UXivSimBuffForceMove, ForceMoveDirection);
}

void UXivSimBuffForceMove::EnableForceMove()
{
	if(!bIgnoreAntiBuff && IsValid(AntiBuffClass))
	{
		if(GetOwnerBuffComponent()->HasBuff(AntiBuffClass))
		{
			return;
		}
	}
	
	bIsForceMoveEnabled = true;

	if (GetOwnerBuffComponent()->GetOwner())
	{
		if (UXivSimPlayerMovementComponent* MoveComp = GetMovementComponent())
		{
			FVector MoveDir = ForceMoveDirection;
			MoveDir.Normalize();
			if (bOwnerPivot)
			{
				MoveDir = GetOwnerBuffComponent()->GetOwner()->GetActorTransform().TransformVector(MoveDir);
			}
			MoveComp->SetForceMove(MoveDir * ForceMoveSpeed);
		}
	}
}

void UXivSimBuffForceMove::ClearForceMove()
{
	bIsForceMoveEnabled = false;
	if (UXivSimPlayerMovementComponent* MoveComp = GetMovementComponent())
	{
		MoveComp->ClearForceMove();
		MoveComp->StopMovementImmediately();
	}
}

void UXivSimBuffForceMove::OnRep_ForceMoveDirection()
{
	EnableForceMove();
}

class UXivSimPlayerMovementComponent* UXivSimBuffForceMove::GetMovementComponent()
{
	if (GetOwnerBuffComponent())
	{
		if (AXivSimCharacter* PlayerOwner = Cast<AXivSimCharacter>(GetOwnerBuffComponent()->GetOwner()))
		{
			return Cast<UXivSimPlayerMovementComponent>(PlayerOwner->GetMovementComponent());
		}
	}

	return nullptr;
}

