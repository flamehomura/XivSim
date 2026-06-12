#include "XivSimActionComponent.h"

#include "XivSimActionBase.h"
#include "XivSimActionManager.h"
#include "XivSimCharacter.h"
#include "Net/UnrealNetwork.h"

UXivSimActionComponent::UXivSimActionComponent()
{
	PrimaryComponentTick.TickGroup = TG_PostPhysics;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.SetTickFunctionEnable(true);

	GlobalCoolingDownTime = 0.f;
	ActionCastingTime.Clear();
	CachedActionInput.Clear();
}

void UXivSimActionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	UpdateActionCoolingDown(DeltaTime);
	UpdateActionCasting(DeltaTime);
}

void UXivSimActionComponent::TryActivateAction(const int32& InActionID, class AXivSimCharacterBase* TargetChar)
{
	if(GetOwnerPlayer() && GetOwnerPlayer()->IsLocallyControlled() && GetOwnerPlayer()->IsPlayerControlled())
	{
		if(GetOwnerPlayer()->IsStunning())
		{
			return;
		}
		
		if(const FXivSimActionData* ActionData = GetActionData(InActionID))
		{
			if(!IsCasting() && !CheckActionInCD(*ActionData) && CheckActionTarget(*ActionData, TargetChar))
			{
				ServerActivateAction(InActionID, TargetChar);
			}
		}
	}
	CachedActionInput.Clear();
}

void UXivSimActionComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UXivSimActionComponent, CurrentActionInfo);
}

void UXivSimActionComponent::ServerActivateAction_Implementation(const int32& InActionID, class AXivSimCharacterBase* TargetChar)
{
	if(const FXivSimActionData* ActionData = GetActionData(InActionID))
	{
		if(!GetOwnerPlayer() || GetOwnerPlayer()->IsStunning())
		{
			return;
		}
		
		if(IsCasting())
		{
			return;
		}
		
		if(CheckActionInCD(*ActionData))
		{
			return;
		}

		if(!CheckActionTarget(*ActionData, TargetChar))
		{
			return;
		}

		ExecuteAction(*ActionData, TargetChar);
	}
}

bool UXivSimActionComponent::CheckActionInCD(const FXivSimActionData& InActionData, float ToleranceTime)
{
	if(InActionData.bWeaponAction)
	{
		if(CheckInGlobalCD(ToleranceTime))
		{
			return true;
		}
	}
		
	if(ActionCoolingDownTimeMap.Contains(InActionData.ActionID))
	{
		return ActionCoolingDownTimeMap[InActionData.ActionID] - GetCDCheckThreshold() > ToleranceTime;
	}

	return false;
}

bool UXivSimActionComponent::CheckActionTarget(const FXivSimActionData& InActionData, AXivSimCharacterBase*& InTarget)
{
	if(!InActionData.bNeedTarget)
	{
		return true;
	}

	if(InTarget == nullptr)
	{
		if(InActionData.bAffectSelf)
		{
			InTarget = GetOwnerPlayer();
		}
		else
		{
			return false;
		}
	}
	
	if(!InActionData.bAffectFriendly && FGenericTeamId::GetAttitude(GetOwnerPlayer(), InTarget) == ETeamAttitude::Type::Friendly)
	{
		return false;
	}

	if(!InActionData.bAffectHostile && FGenericTeamId::GetAttitude(GetOwnerPlayer(), InTarget) == ETeamAttitude::Type::Hostile)
	{
		return false;
	}

	if(InTarget != GetOwnerPlayer())
	{
		const FVector2D TargetLoc2D = FVector2D(InTarget->GetActorLocation());
		const FVector2D PlayerLoc2D = FVector2D(GetOwnerPlayer()->GetActorLocation());
		const float TargetRadius = InTarget->GetCharacterHitRadius();
		const float TargetDistanceSquared = (PlayerLoc2D - TargetLoc2D).SizeSquared();

		if(TargetDistanceSquared > FMath::Square(TargetRadius + InActionData.ActionRange + GetRangeCheckThreshold()))
		{
			return false;
		}
	}
	
	return true;
}

float UXivSimActionComponent::GetActionCDPercent(const FXivSimActionData& InActionData)
{
	if(InActionData.bWeaponAction)
	{
		if(GlobalCoolingDownTime > 0.f && InActionData.RecastTime > 0.f)
		{
			return FMath::Clamp(InActionData.RecastTime - GlobalCoolingDownTime, 0.f, InActionData.RecastTime)
				/ InActionData.RecastTime;
		}
	}
		
	if(ActionCoolingDownTimeMap.Contains(InActionData.ActionID))
	{
		if(ActionCoolingDownTimeMap[InActionData.ActionID] > 0.f && InActionData.RecastTime > 0.f)
		{
			return FMath::Clamp(InActionData.RecastTime - ActionCoolingDownTimeMap[InActionData.ActionID],
				0.f, InActionData.RecastTime) / InActionData.RecastTime;
		}
	}

	return 1.f;
}

void UXivSimActionComponent::TryCacheActionInput(const int32& InActionID, AXivSimCharacterBase* TargetChar)
{
	if(const FXivSimActionData* ActionData = GetActionData(InActionID))
	{
		bool bNeedCacheInput = false;
		bNeedCacheInput = !CheckActionInCD(*ActionData, ActionInputCacheToleranceTime);
		bNeedCacheInput &= !IsCasting(ActionInputCacheToleranceTime);

		if(bNeedCacheInput)
		{
			CachedActionInput.ActionID = InActionID;
			CachedActionInput.ActionTarget = TargetChar;
		}
	}
}

bool UXivSimActionComponent::IsCasting(float ToleranceTime) const
{
	return ActionCastingTime.IsCasting(ToleranceTime);
}

void UXivSimActionComponent::OnRegister()
{
	Super::OnRegister();
	if (USceneComponent* SC = GetOwner()->GetRootComponent())
	{
		SC->TransformUpdated.AddUObject(this, &UXivSimActionComponent::TransformUpdated);
	}
}

void UXivSimActionComponent::OnUnregister()
{
	if (USceneComponent* SC = GetOwner()->GetRootComponent())
	{
		SC->TransformUpdated.RemoveAll(this);
	}
	Super::OnUnregister();
}

AXivSimCharacter* UXivSimActionComponent::GetOwnerPlayer()
{
	return Cast<AXivSimCharacter>(GetOwner());
}

void UXivSimActionComponent::TransformUpdated(USceneComponent* InRootComponent,
	EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	CheckCastingInterrupted();
}

void UXivSimActionComponent::OnRep_CurrentActionInfo()
{
	if(const FXivSimActionData* CurActionData = GetActionData(CurrentActionInfo.ActionID))
	{
		if(UXivSimActionManager* ActionManager = UXivSimActionManager::Get(this))
		{
			if(UXivSimActionBase* ActionObj = ActionManager->GetActionObject(CurrentActionInfo.ActionID))
			{
				if(CurrentActionInfo.bCancelled)
				{
					ActionObj->CancelAction(GetOwnerPlayer());
				}
				else
				{
					ActionObj->ActivateAction(GetOwnerPlayer(), CurrentActionInfo.ActionTargetChar);
				}
			}
		}

		if(!CurrentActionInfo.bCancelled)
		{
			if(CurActionData->bWeaponAction)
			{
				ActivateGlobalCoolingDown(CurActionData->RecastTime);
			}
			else
			{
				ActivateActionCoolingDown(CurActionData->ActionID, CurActionData->RecastTime);
			}
		}
		
		if(CurActionData->bCastingAction)
		{
			if(CurrentActionInfo.bCancelled)
			{
				ActionCastingTime.Clear();
				OnActionCancelCasting.Broadcast(*CurActionData);
			}
			else
			{
				ActivateActionCasting(CurActionData->ActionID, CurActionData->CastingTime, CurrentActionInfo.ActionTargetChar);
				OnActionStartCasting.Broadcast(*CurActionData);
			}
		}
	}
}

void UXivSimActionComponent::ActivateGlobalCoolingDown(float CDTime)
{
	GlobalCoolingDownTime = CDTime;
}

void UXivSimActionComponent::ActivateActionCoolingDown(const int32& ActionID, float CDTime)
{
	float& ExsitCD = ActionCoolingDownTimeMap.FindOrAdd(ActionID);
	ExsitCD = CDTime;
}

void UXivSimActionComponent::ActivateActionCasting(const int32& ActionID, float CastingTime, AXivSimCharacterBase* InTarget)
{
	if(GetOwnerPlayer())
	{
		ActionCastingTime.ActionID = ActionID;
		ActionCastingTime.CastingTime = CastingTime;
		ActionCastingTime.CastingTransform = GetOwnerPlayer()->GetActorTransform();
		if(ActionCastingTime.CastingTarget)
		{
			ActionCastingTime.CastingTarget->OnCanBeTargetedStateChanged.RemoveDynamic(this, &UXivSimActionComponent::OnCastingTargetCanBeSelectedStateChanged);
		}
		ActionCastingTime.CastingTarget = InTarget;
		if(ActionCastingTime.CastingTarget)
		{
			ActionCastingTime.CastingTarget->OnCanBeTargetedStateChanged.AddUniqueDynamic(this, &UXivSimActionComponent::OnCastingTargetCanBeSelectedStateChanged);
		}
	}
}

const FXivSimActionData* UXivSimActionComponent::GetActionData(const int32& InActionID) const
{
	if(UXivSimActionManager* ActionManager = UXivSimActionManager::Get(this))
	{
		return ActionManager->GetActionData(InActionID);
	}

	return nullptr;
}

void UXivSimActionComponent::UpdateActionCoolingDown(const float& DeltaTime)
{
	if(GlobalCoolingDownTime > 0.f)
	{
		GlobalCoolingDownTime = FMath::Max(0.f, GlobalCoolingDownTime - DeltaTime);
		if(GlobalCoolingDownTime <= 0.f)
		{
			OnGlobalCoolingDownFinished();
		}
	}
	
	ActionCoolingDownTimeOutArray.Empty();
	for(auto& CDPair : ActionCoolingDownTimeMap)
	{
		CDPair.Value -= DeltaTime;
		if(CDPair.Value <= 0.f)
		{
			ActionCoolingDownTimeOutArray.Add(CDPair.Key);
		}
	}
	for(const int32& ActionID : ActionCoolingDownTimeOutArray)
	{
		ActionCoolingDownTimeMap.Remove(ActionID);
		OnActionCoolingDownFinished(ActionID);
	}
}

void UXivSimActionComponent::UpdateActionCasting(const float& DeltaTime)
{
	if(ActionCastingTime.IsCasting())
	{
		ActionCastingTime.CastingTime = FMath::Max(0.f, ActionCastingTime.CastingTime - DeltaTime);
		if(ActionCastingTime.CastingTime <= 0.f)
		{
			EndCurrentCasting();
		}
	}
}

void UXivSimActionComponent::OnGlobalCoolingDownFinished()
{
	if(CachedActionInput.IsValid())
	{
		TryActivateAction(CachedActionInput.ActionID, CachedActionInput.ActionTarget);
	}
}

void UXivSimActionComponent::OnActionCoolingDownFinished(const int32& ActionID)
{
	if(CachedActionInput.IsValid())
	{
		TryActivateAction(CachedActionInput.ActionID, CachedActionInput.ActionTarget);
	}
}

bool UXivSimActionComponent::CheckInGlobalCD(float ToleranceTime) const
{
	return GlobalCoolingDownTime - GetCDCheckThreshold() > ToleranceTime;
}

void UXivSimActionComponent::ExecuteAction(const FXivSimActionData& InActionData, class AXivSimCharacterBase* InTarget)
{
	CurrentActionInfo.ActionID = InActionData.ActionID;
	CurrentActionInfo.ActionProcessID++;
	CurrentActionInfo.bCancelled = false;
	if(IsValid(InTarget))
	{
		CurrentActionInfo.ActionTargetChar = InTarget;
	}
	else
	{
		CurrentActionInfo.ActionTargetChar = GetOwnerPlayer();
	}

	OnRep_CurrentActionInfo();
}

float UXivSimActionComponent::GetCDCheckThreshold() const
{
	if(IsNetMode(NM_DedicatedServer))
	{
		return DSActionCDCheckThreshold;
	}
	return 0.f;
}

float UXivSimActionComponent::GetRangeCheckThreshold() const
{
	if(IsNetMode(NM_DedicatedServer))
	{
		return DSActionRangeCheckThreshold;
	}
	return 0.f;
}

bool UXivSimActionComponent::CastingCanBeInterrupted() const
{
	if(IsCasting())
	{
		const FXivSimActionData* CastingActionData = GetActionData(ActionCastingTime.ActionID);
		if(CastingActionData)
		{
			return ActionCastingTime.CastingTime > CastingActionData->CastingInterruptibleToleranceTime;
		}
	}

	return false;
}

void UXivSimActionComponent::CheckCastingInterrupted()
{
	if(GetOwnerPlayer() && GetOwnerPlayer()->HasAuthority())
	{
		if(IsCasting() && CastingCanBeInterrupted())
		{
			if(!GetOwnerPlayer()->GetActorLocation().Equals(ActionCastingTime.CastingTransform.GetLocation()))
			{
				InterruptCurrentCasting();
			}
		}
	}
}

void UXivSimActionComponent::InterruptCurrentCasting()
{
	CurrentActionInfo.ActionID = ActionCastingTime.ActionID;
	CurrentActionInfo.ActionProcessID++;
	CurrentActionInfo.bCancelled = true;

	OnRep_CurrentActionInfo();
}

void UXivSimActionComponent::EndCurrentCasting()
{
	if(const FXivSimActionData* CurActionData = GetActionData(ActionCastingTime.ActionID))
	{
		if(UXivSimActionManager* ActionManager = UXivSimActionManager::Get(this))
		{
			if(UXivSimActionBase* ActionObj = ActionManager->GetActionObject(ActionCastingTime.ActionID))
			{
				ActionObj->EndAction(GetOwnerPlayer());
			}
		}
	}
	
	ActionCastingTime.Clear();
	
	if(CachedActionInput.IsValid())
	{
		TryActivateAction(CachedActionInput.ActionID, CachedActionInput.ActionTarget);
	}
}

void UXivSimActionComponent::OnCastingTargetCanBeSelectedStateChanged(AXivSimCharacterBase* TargetChar, bool bNewState)
{
	if(TargetChar)
	{
		if(TargetChar->HasAuthority())
		{
			if(TargetChar == ActionCastingTime.CastingTarget && IsCasting())
			{
				if(!bNewState)
				{
					InterruptCurrentCasting();
				}
			}
		}
	}
}
