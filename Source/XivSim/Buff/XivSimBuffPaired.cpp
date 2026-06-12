#include "XivSimBuffPaired.h"
#include "BuffComponent.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

UXivSimBuffPaired::UXivSimBuffPaired()
{

}

void UXivSimBuffPaired::OnDeactivated(bool bClientSimulated)
{
	if(ChainFXSystemComponent)
	{
		ChainFXSystemComponent->DestroyComponent();
		ChainFXSystemComponent = nullptr;
	}
	Super::OnDeactivated(bClientSimulated);
}

void UXivSimBuffPaired::TickBuff(float DeltaSeconds)
{
	UpdateFXEffectTarget();
	Super::TickBuff(DeltaSeconds);
}

void UXivSimBuffPaired::SetPairedBuff(class UXivSimBuffPaired* Buff)
{
	PairedBuff = Buff;
}

void UXivSimBuffPaired::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UXivSimBuffPaired, PairedBuff);
	DOREPLIFETIME_CONDITION(UXivSimBuffPaired, bIsLeader, COND_InitialOnly);
}

void UXivSimBuffPaired::CreateFXEffect()
{
	Super::CreateFXEffect();
	if (bIsLeader)
	{
		CreateChainFXEffect();
	}
}

void UXivSimBuffPaired::CreateChainFXEffect()
{
	if (GetWorld()->IsNetMode(NM_DedicatedServer))
	{
		return;
	}

	if (ChainFXSystemTemplete && OwnerBuffComponent)
	{
		ChainFXSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			ChainFXSystemTemplete,
			OwnerBuffComponent->GetOwner()->GetRootComponent(),
			FName(""),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::Type::SnapToTarget,
			false);
		if (ChainFXSystemComponent)
		{
			UpdateFXEffectTarget();
		}
	}
}

void UXivSimBuffPaired::UpdateFXEffectTarget()
{
	if (!GetWorld()->IsNetMode(NM_DedicatedServer))
	{
		if (ChainFXSystemComponent && bIsLeader)
		{
			if (OwnerBuffComponent)
			{
				FVector TargetLocation = OwnerBuffComponent->GetOwner()->GetActorLocation();
				if (PairedBuff)
				{
					if (UBuffComponent* PairedBuffComponent = PairedBuff->GetOwnerBuffComponent())
					{
						if (PairedBuffComponent->IsOwnerValid())
						{
							TargetLocation = PairedBuffComponent->GetOwner()->GetActorLocation();
						}
					}
				}
				ChainFXSystemComponent->SetVectorParameter(FName("TraceEnd"), TargetLocation);
			}
		}
	}
}

class UBuffComponent* UXivSimBuffPaired::GetPairedBuffComponent()
{
	if (PairedBuff)
	{
		return  PairedBuff->GetOwnerBuffComponent();
	}

	return nullptr;
}

