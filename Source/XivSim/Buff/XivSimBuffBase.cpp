#include "XivSimBuffBase.h"
#include "XivSimCharacter.h"
#include "BuffComponent.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/Texture2D.h"

UXivSimBuffBase::UXivSimBuffBase()
{
	bIsActive = false;
	bAllowStack = false;
	bAllowOverwrite = true;
	bPenetrateGodMode = true;
	bAttentionBuff = false;
	FXDeactiveDelayTime = 0.f;
	bFXDeactivated = false;
	StackCount = 1;
	bWantToReplicateRemainingTime = false;
}

void UXivSimBuffBase::Refresh()
{
	SetRemainingTime(MaxTime, true);
}

void UXivSimBuffBase::OnActivated(bool bClientSimulated)
{
	if (HasAuthority())
	{
		bIsActive = true;
	}
	CreateFXEffect();
	OnActivatedBP(bClientSimulated);
}

void UXivSimBuffBase::OnDeactivated(bool bClientSimulated)
{
	if (HasAuthority())
	{
		bIsActive = false;
	}

	if (FXSystemComponent)
	{
		FXSystemComponent->DestroyComponent();
		FXSystemComponent = NULL;
	}
	OnDeactivatedBP(bClientSimulated);
	MarkAsGarbage();
}

void UXivSimBuffBase::OnTimeOver()
{
	OnTimeOver_BP();
	if (HasAuthority())
	{
		SetRemainingTime(-1.f, true);
		DeactivateSelf();
	}
}

void UXivSimBuffBase::DeactivateSelf()
{
	if (OwnerBuffComponent)
	{
		OwnerBuffComponent->RemoveBuff(this);
	}
}

void UXivSimBuffBase::TickBuff(float DeltaSeconds)
{
	TickBuffRemainingTime(DeltaSeconds);
	OnTickBP(DeltaSeconds);
}

void UXivSimBuffBase::SetMaxTime(float NewTime)
{
	MaxTime = NewTime;
	Refresh();
}

float UXivSimBuffBase::GetMaxTime() const
{
	return MaxTime;
}

void UXivSimBuffBase::AddStack(int32 InStackCount)
{
	StackCount += InStackCount;
	OnRep_StackCount();
	if (InStackCount != 0)
	{
		OnStackChangedBP();
	}
}

class UTexture2D* UXivSimBuffBase::GetBuffIcon() const
{
	return BuffIconTexture;
}

float UXivSimBuffBase::GetRemainingTime() const
{
	return LocalRemainingTime;
}

AActor* UXivSimBuffBase::GetOwnerActor()
{
	return Cast<AActor>(GetOuter());
}

void UXivSimBuffBase::SetOwnerBuffComponent(class UBuffComponent* BuffComp)
{
	OwnerBuffComponent = BuffComp;
}

bool UXivSimBuffBase::IsSupportedForNetworking() const
{
	return true;
}

void UXivSimBuffBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UXivSimBuffBase, OwnerBuffComponent, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(UXivSimBuffBase, RemainingTime, COND_InitialOnly);
	DOREPLIFETIME(UXivSimBuffBase, bIsActive)
	DOREPLIFETIME(UXivSimBuffBase, StackCount)
	DOREPLIFETIME(UXivSimBuffBase, bFXDeactivated)
}

void UXivSimBuffBase::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	// DOREPLIFETIME_ACTIVE_OVERRIDE(UXivSimBuffBase, RemainingTime, bWantToReplicateRemainingTime);
	// if(bWantToReplicateRemainingTime)
	// {
	// 	bWantToReplicateRemainingTime = false;
	// }
}

UNiagaraComponent* UXivSimBuffBase::GetOwnedFXComponent() const
{
	return FXSystemComponent;
}

void UXivSimBuffBase::ProcessDamageModifier(float& DamageAmount, const TSubclassOf<class UDamageType>& DamageTypeClass)
{
	DamageAmount = ProcessDamageModifierBP(DamageAmount, DamageTypeClass);
}

float UXivSimBuffBase::ProcessDamageModifierBP_Implementation(float DamageAmount, TSubclassOf<class UDamageType> DamageTypeClass)
{
	return DamageAmount;
}

bool UXivSimBuffBase::HasAuthority()
{
	if (OwnerBuffComponent && OwnerBuffComponent->GetOwner())
	{
		return OwnerBuffComponent->GetOwner()->HasAuthority();
	}

	return false;
}

bool UXivSimBuffBase::IsDedicatedServer()
{
	if(OwnerBuffComponent)
	{
		return OwnerBuffComponent->IsNetMode(NM_DedicatedServer);
	}

	return false;
}

class AXivSimCharacter* UXivSimBuffBase::GetPlayerOwner()
{
	if (OwnerBuffComponent)
	{
		return Cast<AXivSimCharacter>(OwnerBuffComponent->GetOwner());
	}

	return nullptr;
}

void UXivSimBuffBase::CreateFXEffect()
{
	if (GetWorld()->IsNetMode(NM_DedicatedServer))
	{
		return;
	}

	if (FXSystemTemplete && OwnerBuffComponent)
	{
		FXSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			FXSystemTemplete,
			OwnerBuffComponent->GetOwner()->GetRootComponent(),
			FName(""),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::Type::SnapToTarget,
			false);
		if (FXSystemComponent)
		{

		}
	}
}

void UXivSimBuffBase::TickBuffRemainingTime(float DeltaSeconds)
{
	if (HasAuthority())
	{
		if (RemainingTime > 0 && RemainingTime < DeltaSeconds)
		{
			OnTimeOver();
		}
		else
		{
			SetRemainingTime(FMath::Max(0.f, RemainingTime - DeltaSeconds));
		}

		if (FXDeactiveDelayTime > 0.f && MaxTime - RemainingTime > FXDeactiveDelayTime)
		{
			if (!bFXDeactivated)
			{
				bFXDeactivated = true;
				OnRep_FXDeactivated();
			}
		}
	}
	else
	{
		LocalRemainingTime = FMath::Max(0.f, LocalRemainingTime - DeltaSeconds);
	}
}

void UXivSimBuffBase::SetRemainingTime(float NewValue, bool bSetReplicate)
{
	RemainingTime = NewValue;
	if(bSetReplicate)
	{
		bWantToReplicateRemainingTime = true;
	}
	OnRep_RemainingTime();
}

void UXivSimBuffBase::OnRep_RemainingTime()
{
	if(GetPlayerOwner() && !GetPlayerOwner()->IsNetMode(NM_DedicatedServer))
	{
		LocalRemainingTime = RemainingTime;
	}
}

void UXivSimBuffBase::OnRep_StackCount()
{
	if(GetPlayerOwner() && !GetPlayerOwner()->IsNetMode(NM_DedicatedServer))
	{
		LocalRemainingTime = RemainingTime;
	}
}

void UXivSimBuffBase::OnRep_FXDeactivated()
{
	if (!GetWorld()->IsNetMode(NM_DedicatedServer))
	{
		if (FXSystemComponent)
		{
			FXSystemComponent->DeactivateImmediate();
			FXSystemComponent->SetVisibility(false);
		}
	}
}

