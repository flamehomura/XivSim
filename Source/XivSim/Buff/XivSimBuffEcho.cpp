#include "XivSimBuffEcho.h"
#include "BuffComponent.h"

UXivSimBuffEcho::UXivSimBuffEcho()
{

}

void UXivSimBuffEcho::OnDeactivated(bool bClientSimulated)
{
	if (HasAuthority())
	{
		if (OwnerPunishBuff)
		{
			OwnerPunishBuff->DeactivateSelf();
			OwnerPunishBuff = nullptr;
		}

		if (PairedPunishBuff)
		{
			PairedPunishBuff->DeactivateSelf();
			PairedPunishBuff = nullptr;
		}
	}
	Super::OnDeactivated(bClientSimulated);
}

void UXivSimBuffEcho::TickBuff(float DeltaSeconds)
{
	CheckEchoEffect();
	Super::TickBuff(DeltaSeconds);
}

void UXivSimBuffEcho::TriggerPunish(bool bAdd)
{
	if (PunishBuffClass)
	{
		if (bAdd)
		{
			if (!OwnerPunishBuff && GetOwnerBuffComponent())
			{
				OwnerPunishBuff = GetOwnerBuffComponent()->AddBuffByClass(PunishBuffClass, GetOuter());
			}

			if (!PairedPunishBuff && GetPairedBuffComponent())
			{
				PairedPunishBuff = GetPairedBuffComponent()->AddBuffByClass(PunishBuffClass, GetOuter());
			}
		}
		else
		{
			if (OwnerPunishBuff && GetOwnerBuffComponent())
			{
				GetOwnerBuffComponent()->RemoveBuff(OwnerPunishBuff);
				OwnerPunishBuff = nullptr;
			}

			if (PairedPunishBuff && GetPairedBuffComponent())
			{
				GetPairedBuffComponent()->RemoveBuff(PairedPunishBuff);
				PairedPunishBuff = nullptr;
			}
		}
	}
}

void UXivSimBuffEcho::CheckEchoEffect()
{
	if (HasAuthority())
	{
		if (bIsLeader)
		{
			if (EchoType == EXivSimBuffEchoType::EchoType_Far)
			{
				TriggerPunish(GetDistanceToTarget() < MinLimitDistance);
			}
			else if (EchoType == EXivSimBuffEchoType::EchoType_Near)
			{
				TriggerPunish(GetDistanceToTarget() > MaxLimitDistance);
			}
			else if (EchoType == EXivSimBuffEchoType::EchoType_Mid)
			{
				TriggerPunish(GetDistanceToTarget() > MaxLimitDistance || GetDistanceToTarget() < MinLimitDistance);
			}
		}
	}
}

float UXivSimBuffEcho::GetDistanceToTarget()
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

