#include "XivSimPatternBase.h"
#include "Net/UnrealNetwork.h"
#include "XivSimPatternManager.h"
#include "XivSimPlayerManager.h"
#include "XivSimCharacter.h"
#include "XivSimGameState.h"
#include "TimerManager.h"
#include "Components/DecalComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "XivSimDamageType.h"
#include "Engine/DamageEvents.h"
#include "BuffComponent.h"
#include "XivSimBuffBase.h"

UXivSimPatternBase::UXivSimPatternBase()
{
	PatternOwner = nullptr;
	CurrentPatternPhase = EPatternPhase::Phase_None;
	BaseDecalSize = 0.5f;
	BaseDecalDepth = 200.f;
	AffectTransform = FTransform::Identity;
	AffectTransform.SetScale3D(FVector::ZeroVector);
	bCriticalEffect = false;
}

void UXivSimPatternBase::OnActivated()
{
	if (HasAuthority())
	{
		BeginPreparingPhase();
	}
}

void UXivSimPatternBase::OnDeactivated()
{
	if (IsValid(RangeDecal))
	{
		RangeDecal->DestroyComponent();
		RangeDecal = nullptr;
	}
	MarkAsGarbage();
}

bool UXivSimPatternBase::IsSupportedForNetworking() const
{
	return true;
}

void UXivSimPatternBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UXivSimPatternBase, PatternID, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(UXivSimPatternBase, PatternOwner, COND_InitialOnly);
	DOREPLIFETIME(UXivSimPatternBase, PatternTarget);
	DOREPLIFETIME(UXivSimPatternBase, AffectTransform);
	DOREPLIFETIME(UXivSimPatternBase, bCriticalEffect);
	DOREPLIFETIME(UXivSimPatternBase, CurrentPatternPhase);
}

void UXivSimPatternBase::SetPatternID(int32 InID)
{
	PatternID = InID;
	if (UXivSimPatternManager* PatternManager = UXivSimPatternManager::Get(this))
	{
		DataInfo = PatternManager->GetPatternData(PatternID);
	}
}

int32 UXivSimPatternBase::GetPatternID() const
{
	return PatternID;
}

void UXivSimPatternBase::SetPatternTransform(FTransform Trans)
{
	WorldTransform = Trans;
}

void UXivSimPatternBase::SetPatternOwner(AActor* InOwner)
{
	if (InOwner)
	{
		PatternOwner = InOwner;
	}
}

void UXivSimPatternBase::SetPatternTarget(AActor* InTarget)
{
	if (InTarget)
	{
		PatternTarget = InTarget;
	}
}

void UXivSimPatternBase::SetExcludePlayerList(const TArray<class AXivSimCharacter*>& InPlayerList)
{
	ExcludePlayers = InPlayerList;
}

void UXivSimPatternBase::BeginPreparingPhase()
{
	if (HasAuthority())
	{
		CurrentPatternPhase = EPatternPhase::Phase_Preparing;
		BeginPreparingPhaseBP();
		SetTimerToNextPhase();
	}
}

void UXivSimPatternBase::BeginShowRangePhase()
{
	if (HasAuthority())
	{
		CurrentPatternPhase = EPatternPhase::Phase_ShowRange;

		if (PatternOwner)
		{
			if (PatternTarget)
			{
				FTransform NewTransform;
				NewTransform.SetLocation(PatternOwner->GetActorLocation());
				NewTransform.SetRotation(FRotationMatrix::MakeFromZX(FVector(0, 0, 1), PatternTarget->GetActorLocation() - PatternOwner->GetActorLocation()).ToQuat());
				NewTransform.SetScale3D(FVector::OneVector);
				AffectTransform = NewTransform;
			}
			else
			{
				AffectTransform = PatternOwner->GetActorTransform();
			}
		}
		else
		{
			if (PatternTarget)
			{
				FTransform NewTransform;
				NewTransform.SetLocation(WorldTransform.GetLocation());
				NewTransform.SetRotation(FRotationMatrix::MakeFromZX(FVector(0, 0, 1), PatternTarget->GetActorLocation() - WorldTransform.GetLocation()).ToQuat());
				NewTransform.SetScale3D(FVector::OneVector);
				AffectTransform = NewTransform;
			}
			else
			{
				AffectTransform = WorldTransform;
			}
		}

		BeginShowRangePhaseBP();
	}

	if (!GetWorld()->IsNetMode(NM_DedicatedServer))
	{
		if (AffectTransform.GetScale3D() != FVector::ZeroVector)
		{
			if (DataInfo)
			{
				CreateRangeDecal(DataInfo->RangeDecalMaterial, DataInfo->PatternDamageDelay);
			}
		}
	}

	if (HasAuthority())
	{
		SetTimerToNextPhase();
	}
}

void UXivSimPatternBase::BeginLockTargetPhase()
{
	if (HasAuthority())
	{
		CurrentPatternPhase = EPatternPhase::Phase_TargetLock;
		if (DataInfo)
		{
			if (UXivSimPlayerManager* PlayerManager = UXivSimPlayerManager::Get(this))
			{
				if (DataInfo->bExcludeOwnerPlayer)
				{
					if (AXivSimCharacter* OwnerPlayer = Cast<AXivSimCharacter>(PatternOwner))
					{
						ExcludePlayers.AddUnique(OwnerPlayer);
					}
				}
				if (DataInfo->bExcludeTargetPlayer)
				{
					if (AXivSimCharacter* TargetPlayer = Cast<AXivSimCharacter>(PatternTarget))
					{
						ExcludePlayers.AddUnique(TargetPlayer);
					}
				}

				if (DataInfo->RangeType == EXivSimPatternRangeType::RangeType_Attach)
				{
					if (AXivSimCharacter* OwnerPlayer = Cast<AXivSimCharacter>(PatternOwner))
					{
						AffectPlayerArray = {OwnerPlayer};
					}
				}
				else if(DataInfo->AffectTargetPriority == EXivSimPatternPriorityType::PatternType_All)
				{
					AffectPlayerArray = PlayerManager->GetRandomPlayerInRange(
						DataInfo->AffectTargetCount,
						DataInfo->MinAffectTargetCount,
						AffectTransform,
						DataInfo->RangeType,
						DataInfo->RangeParam1,
						DataInfo->RangeParam2,
						ExcludePlayers,
						DataInfo->bAllowDuplicatedTarget,
						DataInfo->bAllowOutOfRangeTarget);
				}
				else
				{
					AffectPlayerArray = PlayerManager->GetRandomPlayerInRange(
						XIVSIM_MAX_PLAYER_COUNT,
						DataInfo->MinAffectTargetCount,
						AffectTransform,
						DataInfo->RangeType,
						DataInfo->RangeParam1,
						DataInfo->RangeParam2,
						ExcludePlayers,
						DataInfo->bAllowDuplicatedTarget,
						DataInfo->bAllowOutOfRangeTarget);

					TArray<class AXivSimCharacter*> RestPlayers;
					if(DataInfo->AffectTargetPriority == EXivSimPatternPriorityType::PatternType_Nearest)
					{
						AffectPlayerArray = PlayerManager->GetNearestPlayerInListFrom(
							DataInfo->AffectTargetCount,
							AffectTransform.GetLocation(),
							AffectPlayerArray,
							RestPlayers);
					}
					else if(DataInfo->AffectTargetPriority == EXivSimPatternPriorityType::PatternType_Farest)
					{
						AffectPlayerArray = PlayerManager->GetFarestPlayerInListFrom(
							DataInfo->AffectTargetCount,
							AffectTransform.GetLocation(),
							AffectPlayerArray,
							RestPlayers);
					}
					else if(DataInfo->AffectTargetPriority == EXivSimPatternPriorityType::PatternType_LineNearest)
					{
						AffectPlayerArray = PlayerManager->GetNearestPlayerInListFromLine(
							DataInfo->AffectTargetCount,
							AffectTransform.GetLocation(),
							AffectTransform.GetRotation().GetRightVector(),
							AffectPlayerArray,
							RestPlayers);
					
					}
					else if(DataInfo->AffectTargetPriority == EXivSimPatternPriorityType::PatternType_LineFarest)
					{
						AffectPlayerArray = PlayerManager->GetFarestPlayerInListFromLine(
							DataInfo->AffectTargetCount,
							AffectTransform.GetLocation(),
							AffectTransform.GetRotation().GetRightVector(),
							AffectPlayerArray,
							RestPlayers);
					}
				}

				bCriticalEffect = AffectPlayerArray.Num() > DataInfo->MaxAffectTargetCount || AffectPlayerArray.Num() < DataInfo->MinAffectTargetCount;
				if (bCriticalEffect && DataInfo->bCriticalAffectAll)
				{
					AffectPlayerArray = PlayerManager->GetAllPlayer();
				}
			}
		}

		BeginLockTargetPhaseBP();

		SetTimerToNextPhase();
	}
}

void UXivSimPatternBase::BeginEffectPhase()
{
	if (HasAuthority())
	{
		CurrentPatternPhase = EPatternPhase::Phase_Effect;

		if (DataInfo)
		{
			if (bCriticalEffect)
			{
				int32 DamageAmount = DataInfo->CriticalDamageAmount;
				if (DataInfo->PatternType == EXivSimPatternType::PatternType_Stack)
				{
					int32 TargetCount = AffectPlayerArray.Num();
					DamageAmount = FMath::CeilToInt((float)DamageAmount / (float)TargetCount);
				}
				
				for (AXivSimCharacter* Player : AffectPlayerArray)
				{
					if (DataInfo->CriticalDamageTypeClass)
					{
						Player->TakeDamage(
							DamageAmount,
							FDamageEvent(DataInfo->CriticalDamageTypeClass),
							nullptr,
							PatternOwner == nullptr ? GetWorld()->GetGameState() : PatternOwner);
					}

					for (auto BuffClass : DataInfo->CriticalBuffClasses)
					{
						Player->GetBuffComponent()->AddBuffByClass(BuffClass, PatternOwner == nullptr ? Player : PatternOwner);
					}
				}
			}
			else
			{
				int32 DamageAmount = DataInfo->DamageAmount;
				if (DataInfo->PatternType == EXivSimPatternType::PatternType_Stack)
				{
					int32 TargetCount = AffectPlayerArray.Num();
					DamageAmount = FMath::CeilToInt((float)DamageAmount / (float)TargetCount);
				}

				for (AXivSimCharacter* Player : AffectPlayerArray)
				{
					if (DataInfo->DamageTypeClass)
					{
						Player->TakeDamage(
							DamageAmount,
							FDamageEvent(DataInfo->DamageTypeClass),
							nullptr,
							PatternOwner == nullptr ? GetWorld()->GetGameState() : PatternOwner);
					}

					for (auto BuffClass : DataInfo->BuffClasses)
					{
						Player->GetBuffComponent()->AddBuffByClass(BuffClass, PatternOwner == nullptr ? Player : PatternOwner);
					}
				}
			}
		}

		BeginEffectPhaseBP();
	}

	if (!GetWorld()->IsNetMode(NM_DedicatedServer))
	{
		if (AffectTransform.GetScale3D() != FVector::ZeroVector)
		{
			if (DataInfo)
			{
				CreateRangeDecal(DataInfo->EffectDecalMaterial, DataInfo->PatternEffectLifeSpan);
			}
		}
	}

	if (HasAuthority())
	{
		SetTimerToNextPhase();
	}
}

void UXivSimPatternBase::BeginFieldPhase()
{
	if (HasAuthority())
	{
		CurrentPatternPhase = EPatternPhase::Phase_Field;
		if(DataInfo && DataInfo->PatternFieldLifeSpan > 0.f)
		{
			FTimerDelegate FieldTickTimerHandleDelegate = FTimerDelegate::CreateUObject(this, &UXivSimPatternBase::FieldEffectTick, false);
			GetWorld()->GetTimerManager().SetTimer(FieldTickTimerHandle, FieldTickTimerHandleDelegate, FieldEffectTickInterval, true);
			GetWorld()->GetTimerManager().SetTimer(FieldTickEndTimerHandle, this, &UXivSimPatternBase::FieldEffectTickEnd, DataInfo->PatternFieldLifeSpan, false);
			FieldEffectTick(true);
		}
	}
	
	if (!GetWorld()->IsNetMode(NM_DedicatedServer))
	{
		if (AffectTransform.GetScale3D() != FVector::ZeroVector)
		{
			if (DataInfo)
			{
				CreateRangeDecal(DataInfo->FieldDecalMaterial, DataInfo->PatternFieldLifeSpan, true);
			}
		}
	}

	if (HasAuthority())
	{
		SetTimerToNextPhase();
	}
}

void UXivSimPatternBase::BeginDestroyPhase()
{
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(PhaseChangeTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(FieldTickTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(FieldTickEndTimerHandle);
		if (GetOwnerGameState())
		{
			GetOwnerGameState()->RemovePattern(this);
		}
	}
}

class AXivSimGameState* UXivSimPatternBase::GetOwnerGameState()
{
	if (GetOuter())
	{
		return Cast<AXivSimGameState>(GetOuter());
	}

	return nullptr;
}

bool UXivSimPatternBase::HasAuthority()
{
	if (GetOwnerGameState())
	{
		return GetOwnerGameState()->HasAuthority();
	}

	return false;
}

void UXivSimPatternBase::SetTimerToNextPhase()
{
	if (HasAuthority() && DataInfo)
	{
		GetWorld()->GetTimerManager().ClearTimer(PhaseChangeTimerHandle);
		switch (CurrentPatternPhase)
		{
		case EPatternPhase::Phase_Preparing:
		{
			if (DataInfo->PatternRangeDelay > 0)
			{
				GetWorld()->GetTimerManager().SetTimer(PhaseChangeTimerHandle, this, &UXivSimPatternBase::BeginShowRangePhase, DataInfo->PatternRangeDelay, false);
			}
			else
			{
				BeginShowRangePhase();
			}
		}
		break;
		case EPatternPhase::Phase_ShowRange:
		{
			if (DataInfo->PatternDamageDelay > 0)
			{
				GetWorld()->GetTimerManager().SetTimer(PhaseChangeTimerHandle, this, &UXivSimPatternBase::BeginLockTargetPhase, DataInfo->PatternDamageDelay, false);
			}
			else
			{
				BeginLockTargetPhase();
			}
		}
		break;
		case EPatternPhase::Phase_TargetLock:
		{
			if (DataInfo->PatternEffectDelay > 0)
			{
				GetWorld()->GetTimerManager().SetTimer(PhaseChangeTimerHandle, this, &UXivSimPatternBase::BeginEffectPhase, DataInfo->PatternEffectDelay, false);
			}
			else
			{
				BeginEffectPhase();
			}
		}
		break;
		case EPatternPhase::Phase_Effect:
		{
			if(DataInfo->PatternType == EXivSimPatternType::PatternType_Field)
			{
				if (DataInfo->PatternFieldDelay > 0)
				{
					GetWorld()->GetTimerManager().SetTimer(PhaseChangeTimerHandle, this, &UXivSimPatternBase::BeginFieldPhase, DataInfo->PatternFieldDelay, false);
				}
				else
				{
					BeginFieldPhase();
				}
			}
			else
			{
				if (DataInfo->PatternDestroyDelay > 0)
				{
					GetWorld()->GetTimerManager().SetTimer(PhaseChangeTimerHandle, this, &UXivSimPatternBase::BeginDestroyPhase, DataInfo->PatternDestroyDelay, false);
				}
				else
				{
					BeginDestroyPhase();
				}
			}
		}
		break;

		case EPatternPhase::Phase_Field:
		{
			if (DataInfo->PatternDestroyDelay + DataInfo->PatternFieldLifeSpan > 0)
			{
				GetWorld()->GetTimerManager().SetTimer(PhaseChangeTimerHandle, this, &UXivSimPatternBase::BeginDestroyPhase, DataInfo->PatternDestroyDelay + DataInfo->PatternFieldLifeSpan, false);
			}
			else
			{
				BeginDestroyPhase();
			}
		}
		break;
		}
	}
}

void UXivSimPatternBase::FieldEffectTick(bool bFirstTick)
{
	if(HasAuthority())
	{
		if(DataInfo)
		{
			if (UXivSimPlayerManager* PlayerManager = UXivSimPlayerManager::Get(this))
			{
				if(bFirstTick)
				{
					FieldRangeTolerancePlayerArray = PlayerManager->GetRandomPlayerInRange(
						XIVSIM_MAX_PLAYER_COUNT, 0, AffectTransform, DataInfo->FieldRangeType, DataInfo->FieldRangeParam1,
						DataInfo->FieldRangeParam2, TArray<class AXivSimCharacter*>());

					FieldEffectTickStartTime = GetWorld()->GetTimeSeconds();
				}
				else
				{
					if(GetWorld()->GetTimeSeconds() - FieldEffectTickStartTime > FieldEffectTickToleranceTime)
					{
						FieldRangeTolerancePlayerArray.Empty();
					}
					
					FieldRangePlayerArray = PlayerManager->GetRandomPlayerInRange(
						XIVSIM_MAX_PLAYER_COUNT, 0, AffectTransform, DataInfo->FieldRangeType, DataInfo->FieldRangeParam1,
						DataInfo->FieldRangeParam2, FieldRangeTolerancePlayerArray);

					const int32 DamageAmount = DataInfo->FieldDamageAmount;
					for (AXivSimCharacter* Player : FieldRangePlayerArray)
					{
						if (DamageAmount > 0.f && DataInfo->FieldDamageTypeClass)
						{
							Player->TakeDamage(
								DamageAmount,
								FDamageEvent(DataInfo->DamageTypeClass),
								nullptr,
								PatternOwner == nullptr ? GetWorld()->GetGameState() : PatternOwner);
						}

						for (auto BuffClass : DataInfo->FieldBuffClasses)
						{
							Player->GetBuffComponent()->AddBuffByClass(BuffClass, PatternOwner == nullptr ? Player : PatternOwner);
						}
					}
				}
			}
		}
	}
}

void UXivSimPatternBase::FieldEffectTickEnd()
{
	GetWorld()->GetTimerManager().ClearTimer(FieldTickTimerHandle);
}

void UXivSimPatternBase::CreateRangeDecal(class UMaterialInterface* DecalMaterial, float DecalLifeSpan, bool bFieldDecal)
{
	if (DataInfo && GetWorld()->GetNetMode() != NM_DedicatedServer)
	{
		if (DecalMaterial && DecalLifeSpan > 0.f)
		{
			RangeDecal = NewObject<UDecalComponent>(GetWorld());
			RangeDecal->bAllowAnyoneToDestroyMe = true;
			RangeDecal->SetDecalMaterial(DecalMaterial);
			RangeDecal->DecalSize = FVector(BaseDecalDepth, BaseDecalSize, BaseDecalSize);
			RangeDecal->SetUsingAbsoluteScale(true);
			RangeDecal->RegisterComponentWithWorld(GetWorld());
			RangeDecal->SetLifeSpan(DecalLifeSpan);
			RangeDecal->SetWorldLocation(AffectTransform.GetLocation());
			RangeDecal->SetWorldRotation(FRotationMatrix::MakeFromXY(FVector(0, 0, -1), AffectTransform.GetRotation().GetForwardVector()).Rotator());
			ModifyDecalRange(bFieldDecal);
		}
	}
}

void UXivSimPatternBase::ModifyDecalRange(bool bFieldDecal)
{
	if (DataInfo && GetWorld()->GetNetMode() != NM_DedicatedServer)
	{
		if (RangeDecal)
		{
			float RangeParam1 = bFieldDecal ? DataInfo->FieldRangeParam1 : DataInfo->RangeParam1;
			float RangeParam2 = bFieldDecal ? DataInfo->FieldRangeParam2 : DataInfo->RangeParam2;
			switch (DataInfo->RangeType)
			{
			case EXivSimPatternRangeType::RangeType_Circle:
			{
				RangeDecal->DecalSize.Y = BaseDecalSize * RangeParam1 * 2.f;
				RangeDecal->DecalSize.Z = BaseDecalSize * RangeParam1 * 2.f;
			}
			break;
			case EXivSimPatternRangeType::RangeType_Sector:
			{
				RangeDecal->DecalSize.Y = BaseDecalSize * RangeParam1 * 2.f;
				RangeDecal->DecalSize.Z = BaseDecalSize * RangeParam1 * 2.f;
				if (UMaterialInstanceDynamic* MID = GetRangeDecalDynamicMaterial())
				{
					MID->SetScalarParameterValue(FName("Degree"), RangeParam2);
				}
			}
			break;
			case EXivSimPatternRangeType::RangeType_Line:
			{
				RangeDecal->DecalSize.Y = BaseDecalSize * RangeParam1;
				RangeDecal->DecalSize.Z = BaseDecalSize * RangeParam2;
			}
			break;
			case EXivSimPatternRangeType::RangeType_HalfLine:
			{
				RangeDecal->DecalSize.Y = BaseDecalSize * RangeParam1 * 2.f;
				RangeDecal->DecalSize.Z = BaseDecalSize * RangeParam2;
			}
			break;
			case EXivSimPatternRangeType::RangeType_Ring:
			{
				RangeDecal->DecalSize.Y = BaseDecalSize * RangeParam1 * 2.f;
				RangeDecal->DecalSize.Z = BaseDecalSize * RangeParam1 * 2.f;
				if (UMaterialInstanceDynamic* MID = GetRangeDecalDynamicMaterial())
				{
					MID->SetScalarParameterValue(FName("RingRadius"), RangeParam2 / RangeParam1);
				}
			}
			break;
			case EXivSimPatternRangeType::RangeType_Cross:
			{
				RangeDecal->DecalSize.Y = BaseDecalSize * RangeParam1;
				RangeDecal->DecalSize.Z = BaseDecalSize * RangeParam1;
				if (UMaterialInstanceDynamic* MID = GetRangeDecalDynamicMaterial())
				{
					MID->SetScalarParameterValue(FName("CrossWidth"), RangeParam2 / RangeParam1);
				}
			}
			break;
			case EXivSimPatternRangeType::RangeType_Wing:
			{
				RangeDecal->DecalSize.Y = BaseDecalSize * RangeParam1;
				RangeDecal->DecalSize.Z = BaseDecalSize * RangeParam1;
				if (UMaterialInstanceDynamic* MID = GetRangeDecalDynamicMaterial())
				{
					MID->SetScalarParameterValue(FName("WingWidth"), RangeParam2 / RangeParam1);
				}
			}
			break;
			case EXivSimPatternRangeType::RangeType_EvilEye:
			case EXivSimPatternRangeType::RangeType_EvilEyeAnti:
				{
					RangeDecal->DecalSize.Y = BaseDecalSize * RangeParam1 * 2.f;
					RangeDecal->DecalSize.Z = BaseDecalSize * RangeParam1 * 2.f;
				}
			break;
			default:
				break;
			}
		}
	}
}

UMaterialInstanceDynamic* UXivSimPatternBase::GetRangeDecalDynamicMaterial()
{
	if (RangeDecal)
	{
		UMaterialInterface* MI = RangeDecal->GetDecalMaterial();
		if (UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(MI))
		{
			return MID;
		}
		else
		{
			return RangeDecal->CreateDynamicMaterialInstance();
		}
	}
	return nullptr;
}

void UXivSimPatternBase::OnRep_PatternID()
{
	SetPatternID(PatternID);
}

void UXivSimPatternBase::OnRep_AffectTransform()
{
	if (CurrentPatternPhase == EPatternPhase::Phase_ShowRange && RangeDecal == nullptr)
	{
		if (DataInfo)
		{
			CreateRangeDecal(DataInfo->RangeDecalMaterial, DataInfo->PatternDamageDelay);
		}
	}
}

void UXivSimPatternBase::OnRep_CurrentPatternPhase()
{
	switch (CurrentPatternPhase)
	{
	case EPatternPhase::Phase_Preparing:
		BeginPreparingPhase();
		break;
	case EPatternPhase::Phase_ShowRange:
		BeginShowRangePhase();
		break;
	case EPatternPhase::Phase_TargetLock:
		BeginLockTargetPhase();
		break;
	case EPatternPhase::Phase_Effect:
		BeginEffectPhase();
		break;
	case EPatternPhase::Phase_Field:
		BeginFieldPhase();
		break;
	}
	
	if (DataInfo && DataInfo->bCriticalPattern)
	{
		if (AXivSimGameState* GameState = GetWorld()->GetGameState<AXivSimGameState>())
		{
			GameState->OnCriticalPatternPhaseChanged.Broadcast(this, CurrentPatternPhase);
		}
	}
}

