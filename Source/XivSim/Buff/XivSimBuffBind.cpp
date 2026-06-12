// Copyright@Homura Akane@Chocobo


#include "XivSimBuffBind.h"

#include "BuffComponent.h"
#include "XivSimCharacter.h"
#include "XivSimPatternManager.h"

class UXivSimPatternManager;

UXivSimBuffBind::UXivSimBuffBind()
{
	InitialDistance = 0.f;
}

void UXivSimBuffBind::TickBuff(float DeltaSeconds)
{
	CheckBindDistance();
	Super::TickBuff(DeltaSeconds);
}

void UXivSimBuffBind::SetPairedBuff(UXivSimBuffPaired* Buff)
{
	Super::SetPairedBuff(Buff);

	InitialDistance = GetDistanceToTarget();
}

void UXivSimBuffBind::CheckBindDistance()
{
	if(HasAuthority())
	{
		if(bIsLeader)
		{
			const float PairDis = GetDistanceToTarget();
			if(PairDis > 0.f)
			{
				if(PairDis > MaxDistance || PairDis < MinDistance)
				{
					TriggerPattern();
					return;
				}
			}
			
			if (GetPairedBuffComponent())
			{
				if (!GetPlayerOwner() || !GetPlayerOwner()->IsAlive() ||
					!GetPairedBuffComponent()->GetPlayerOwner() || !GetPairedBuffComponent()->GetPlayerOwner()->IsAlive())
				{
					TriggerPattern();
					return;
				}
			}
		}
	}
}

float UXivSimBuffBind::GetDistanceToTarget()
{
	if (PairedBuff)
	{
		if (UBuffComponent* PairedBuffComponent = GetPairedBuffComponent())
		{
			if (OwnerBuffComponent)
			{
				FVector OwnerLoc = OwnerBuffComponent->GetOwner()->GetActorLocation();
				FVector TargetLoc = PairedBuffComponent->GetOwner()->GetActorLocation();

				return (OwnerLoc - TargetLoc).Size2D();
			}
		}
	}

	return -1.f;
}

void UXivSimBuffBind::TriggerPattern()
{
	if(HasAuthority())
	{
		if (GetWorld())
		{
			if (CriticalPatternID > 0)
			{
				if (UXivSimPatternManager* PatternManager = UXivSimPatternManager::Get(this))
				{
					PatternManager->ActivatePatternAttach(CriticalPatternID, OwnerBuffComponent->GetOwner());

					if (PairedBuff)
					{
						UBuffComponent* PairedBuffComponent = PairedBuff->GetOwnerBuffComponent();
						if (PairedBuffComponent)
						{
							PatternManager->ActivatePatternAttach(CriticalPatternID, PairedBuffComponent->GetOwner());
						}
					}
				}
			}
		}

		DeactivateSelf();
		if (PairedBuff)
		{
			PairedBuff->DeactivateSelf();
		}
	}
}
