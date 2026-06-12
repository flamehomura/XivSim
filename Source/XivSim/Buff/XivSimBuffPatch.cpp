#include "XivSimBuffPatch.h"
#include "BuffComponent.h"
#include "NiagaraComponent.h"
#include "Net/UnrealNetwork.h"
#include "XivSimPlayerManager.h"
#include "XivSimCharacter.h"
#include "XivSimPatternManager.h"


UXivSimBuffPatch::UXivSimBuffPatch()
{

}

void UXivSimBuffPatch::TickBuff(float DeltaSeconds)
{
	CheckPatchEffect();
	Super::TickBuff(DeltaSeconds);
}

void UXivSimBuffPatch::OnTimeOver()
{
	if (HasAuthority() && bIsLeader)
	{
		if (!bMaterialized && PatchOverBuffClass)
		{
			UBuffComponent* PairedBuffComponent = PairedBuff->GetOwnerBuffComponent();
			if (OwnerBuffComponent && PairedBuffComponent)
			{
				UXivSimBuffPaired* PairedBuffLeader = Cast<UXivSimBuffPaired>(OwnerBuffComponent->AddBuffByClass(PatchOverBuffClass, GetOuter()));
				if (PairedBuffLeader)
				{
					PairedBuffLeader->SetIsLeader(true);
					UXivSimBuffPaired* PairedBuffFollower = Cast<UXivSimBuffPaired>(PairedBuffComponent->AddBuffByClass(PatchOverBuffClass, GetOuter()));
					if (PairedBuffFollower)
					{
						PairedBuffFollower->SetIsLeader(false);
						PairedBuffLeader->SetPairedBuff(PairedBuffFollower);
					}
				}
			}
		}

		if (bMaterialized)
		{
			TriggerPatch();
		}
	}

	Super::OnTimeOver();
}

void UXivSimBuffPatch::TriggerPatch(bool bCritical)
{
	if (HasAuthority())
	{
		if (GetWorld())
		{
			int32 PatternID = bCritical ? PatchCriticalPatternID : PatchPatternID;
			if (PatternID > 0)
			{
				if (UXivSimPatternManager* PatternManager = UXivSimPatternManager::Get(this))
				{
					PatternManager->ActivatePatternAttach(PatternID, OwnerBuffComponent->GetOwner());

					if (PairedBuff)
					{
						UBuffComponent* PairedBuffComponent = PairedBuff->GetOwnerBuffComponent();
						if (PairedBuffComponent)
						{
							PatternManager->ActivatePatternAttach(PatternID, PairedBuffComponent->GetOwner());
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

void UXivSimBuffPatch::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UXivSimBuffPatch::CheckPatchEffect()
{
	if (HasAuthority())
	{
		if (bMaterialized && bIsLeader)
		{
			if (PatchType == EXivSimBuffPatchType::PatchType_Far)
			{
				if (GetDistanceToTarget() > LimitDistance)
				{
					TriggerPatch();
				}
			}
			else if (PatchType == EXivSimBuffPatchType::PatchType_Near)
			{
				if (GetDistanceToTarget() < LimitDistance)
				{
					TriggerPatch();
				}
			}
			else if (GetPairedBuffComponent())
			{
				if (!GetPlayerOwner() || !GetPlayerOwner()->IsAlive() ||
					!GetPairedBuffComponent()->GetPlayerOwner() || !GetPairedBuffComponent()->GetPlayerOwner()->IsAlive())
				{
					TriggerPatch();
				}
			}
		}
	}
}

float UXivSimBuffPatch::GetDistanceToTarget()
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

