#include "BuffComponent.h"
#include "XivSimBuffBase.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "XivSimCharacter.h"

UBuffComponent::UBuffComponent()
{
	PrimaryComponentTick.TickGroup = TG_PostPhysics;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.SetTickFunctionEnable(true);

	bReplicateUsingRegisteredSubObjectList = true;
	BuffContainer.OwnerBuffComponent = this;
}

void UBuffComponent::OnUnregister()
{
	ClearBuffs();
	Super::OnUnregister();
}

void UBuffComponent::AddBuff(class UXivSimBuffBase* Buff)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	if(!IsValid(this))
	{
		return;
	}

	if (Buff)
	{
		if(!Buff->IsPenetrateGodMode() && GetPlayerOwner()->IsInGodMode())
		{
			return;
		}
		Buff->SetOwnerBuffComponent(this);
		Buff->Refresh();
		Buff->OnActivated();
		FBuffInfo NewBuff;
		NewBuff.BuffPtr = Buff;
		BuffContainer.BuffList.Add(NewBuff);
		AddReplicatedSubObject(Buff);
		OnBuffAdd.Broadcast(Buff);
		BuffContainer.MarkArrayDirty();
	}
}

UXivSimBuffBase* UBuffComponent::AddBuffByClass(TSubclassOf<UXivSimBuffBase> BuffClass, UObject* BuffCreater)
{
	if (BuffClass && BuffCreater)
	{
		for (int32 i = 0; i < BuffContainer.BuffList.Num(); ++i)
		{
			UXivSimBuffBase* CheckedBuff = BuffContainer.BuffList[i].BuffPtr;
			if (CheckedBuff)
			{
				if (CheckedBuff->GetClass() == BuffClass)
				{
					if (CheckedBuff->IsAllowOverwrite())
					{
						RemoveBuff(CheckedBuff);
						break;
					}
					else if(CheckedBuff->IsAllowStack())
					{
						CheckedBuff->Refresh();
						CheckedBuff->AddStack(1);
						return CheckedBuff;
					}
				}
			}
		}

		UXivSimBuffBase* NewBuff = NewObject<UXivSimBuffBase>(BuffCreater, BuffClass);
		AddBuff(NewBuff);
		return NewBuff;
	}

	return nullptr;
}

bool UBuffComponent::HasBuff(TSubclassOf<UXivSimBuffBase> BuffClass)
{
	if(IsValid(BuffClass))
	{
		for (int32 i = BuffContainer.BuffList.Num() - 1; i >= 0; --i)
		{
			if (BuffContainer.BuffList[i].BuffPtr && BuffContainer.BuffList[i].BuffPtr->GetClass()->IsChildOf(BuffClass))
			{
				return true;
			}
		}
	}
	
	return false;
}

void UBuffComponent::RemoveBuffByClass(TSubclassOf<UXivSimBuffBase> BuffClass)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	if(!IsValid(BuffClass))
	{
		return;
	}

	for (int32 i = BuffContainer.BuffList.Num() - 1; i >= 0; --i)
	{
		if (BuffContainer.BuffList[i].BuffPtr && BuffContainer.BuffList[i].BuffPtr->GetClass()->IsChildOf(BuffClass))
		{
			if(BuffContainer.BuffList[i].BuffPtr->IsAllowStack())
			{
				BuffContainer.BuffList[i].BuffPtr->Refresh();
				BuffContainer.BuffList[i].BuffPtr->AddStack(-1);
				BuffContainer.MarkItemDirty(BuffContainer.BuffList[i]);
				if (BuffContainer.BuffList[i].BuffPtr->GetStackCount() > 0)
				{
					continue;
				}
			}
			
			BuffContainer.BuffList[i].BuffPtr->OnDeactivated();
			OnBuffRemove.Broadcast(BuffContainer.BuffList[i].BuffPtr);
			RemoveReplicatedSubObject(BuffContainer.BuffList[i].BuffPtr);
			BuffContainer.BuffList.RemoveAt(i);
			BuffContainer.MarkArrayDirty();
		}
	}
}

void UBuffComponent::RemoveBuff(class UXivSimBuffBase* Buff)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	for (int32 i = BuffContainer.BuffList.Num() - 1; i >= 0; --i)
	{
		if (BuffContainer.BuffList[i].BuffPtr == Buff)
		{
			Buff->OnDeactivated();
			OnBuffRemove.Broadcast(Buff);
			RemoveReplicatedSubObject(Buff);
			BuffContainer.BuffList.RemoveAt(i);
			BuffContainer.MarkArrayDirty();
			break;
		}
	}
}

void UBuffComponent::ClearBuffs()
{
	if (GetOwner()->HasAuthority())
	{
		for (int32 i = BuffContainer.BuffList.Num() - 1; i >= 0; --i)
		{
			if (UXivSimBuffBase* Buff = BuffContainer.BuffList[i].BuffPtr)
			{
				Buff->OnDeactivated();
				RemoveReplicatedSubObject(Buff);
				BuffContainer.BuffList.RemoveAt(i);
			}
		}
		BuffContainer.MarkArrayDirty();
	}
}

class AXivSimCharacter* UBuffComponent::GetPlayerOwner() const
{
	return Cast<AXivSimCharacter>(GetOwner());
}

bool UBuffComponent::IsOwnerValid() const
{
	if (AXivSimCharacter* PlayerOwner = GetPlayerOwner())
	{
		return PlayerOwner->IsAlive();
	}

	return false;
}

void UBuffComponent::ProcessDamageModifier(float& DamageAmount, const TSubclassOf<class UDamageType>& DamageTypeClass)
{
	for (int32 i = 0; i < BuffContainer.BuffList.Num(); ++i)
	{
		if (UXivSimBuffBase* CheckedBuff = BuffContainer.BuffList[i].BuffPtr)
		{
			CheckedBuff->ProcessDamageModifier(DamageAmount, DamageTypeClass);
		}
	}
}

void UBuffComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TArray<FBuffInfo> TempBuffArray = BuffContainer.BuffList;

	for (int32 i = TempBuffArray.Num() - 1; i >= 0; --i)
	{
		if(UXivSimBuffBase* Buff = TempBuffArray[i].BuffPtr)
		{
			if (Buff->IsActive())
			{
				Buff->TickBuff(DeltaTime);
			}
		}
	}
}

void UBuffComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBuffComponent, BuffContainer);
}

bool UBuffComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (FBuffInfo BuffInfo : BuffContainer.BuffList)
	{
		if (IsValid(BuffInfo.BuffPtr))
		{
			WroteSomething |= Channel->ReplicateSubobject(BuffInfo.BuffPtr, *Bunch, *RepFlags);
		}
	}
	return WroteSomething;
}

void UBuffComponent::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);
	for (FBuffInfo BuffInfo : BuffContainer.BuffList)
	{
		if (IsValid(BuffInfo.BuffPtr))
		{
			BuffInfo.BuffPtr->PreReplication(ChangedPropertyTracker);
		}
	}
}

void FBuffInfo::PreReplicatedRemove(const struct FBuffInfoContainer& InArraySerializer)
{
	if (BuffPtr)
	{
		InArraySerializer.OwnerBuffComponent->OnBuffRemove.Broadcast(BuffPtr);
		BuffPtr->OnDeactivated(true);
	}
}

void FBuffInfo::PostReplicatedAdd(const struct FBuffInfoContainer& InArraySerializer)
{
	if (BuffPtr)
	{
		BuffPtr->OnActivated(true);
		InArraySerializer.OwnerBuffComponent->OnBuffAdd.Broadcast(BuffPtr);
	}
}

void FBuffInfo::PostReplicatedChange(const struct FBuffInfoContainer& InArraySerializer)
{
	if (BuffPtr)
	{
		BuffPtr->OnActivated(true);
		InArraySerializer.OwnerBuffComponent->OnBuffAdd.Broadcast(BuffPtr);
	}
}
