#include "XivSimGameState.h"
#include "XivSimCharacter.h"
#include "EngineUtils.h"
#include "UObject/ConstructorHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "XivSimDamageType.h"
#include "XivSimPlayerState.h"
#include "XivSimPlayerManager.h"
#include "XivSimPatternBase.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogWorld, Log, All);

AXivSimGameState::AXivSimGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//bReplicateUsingRegisteredSubObjectList = true;
	PatternItemContainer.OwnerGameState = this;
}

void AXivSimGameState::HandleBeginPlay()
{
	Super::HandleBeginPlay();

	if(IsNetMode(NM_DedicatedServer))
	{
		UE_LOG(LogWorld, Warning, TEXT("XivSim Current Player State Count: %d At Port:%d"),
			PlayerArray.Num(), GetWorld()->URL.Port);
	}
}

void AXivSimGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	PatternItemContainer.Clear();
	Super::EndPlay(EndPlayReason);
}

void AXivSimGameState::AddPlayerState(APlayerState* PlayerState)
{
	if (AXivSimPlayerState* XivSimPS = Cast<AXivSimPlayerState>(PlayerState))
	{
		if (!PlayerState->IsInactive())
		{
			if (PlayerArray.Find(PlayerState) == INDEX_NONE)
			{
				PlayerArray.Add(PlayerState);
				if (UXivSimPlayerManager* PlayerManager = UXivSimPlayerManager::Get(this))
				{
					PlayerManager->AddPlayerState(XivSimPS);
					if(IsNetMode(NM_DedicatedServer))
					{
						UE_LOG(LogWorld, Warning, TEXT("XivSim Current Player State Count: %d At Port:%d"),
							PlayerArray.Num(), GetWorld()->URL.Port);
					}
				}
			}
		}
	}
	//UpdateServerWindowTitle();
}

void AXivSimGameState::RemovePlayerState(APlayerState* PlayerState)
{
	if (AXivSimPlayerState* XivSimPS = Cast<AXivSimPlayerState>(PlayerState))
	{
		for (int32 i = 0; i < PlayerArray.Num(); i++)
		{
			if (PlayerArray[i] == PlayerState)
			{
				if (UXivSimPlayerManager* PlayerManager = UXivSimPlayerManager::Get(this))
				{
					PlayerManager->RemovePlayerState(XivSimPS);
				}
				PlayerArray.RemoveAt(i, 1);
				if(IsNetMode(NM_DedicatedServer))
				{
					UE_LOG(LogWorld, Warning, TEXT("XivSim Current Player State Count: %d At Port:%d"),
						PlayerArray.Num(), GetWorld()->URL.Port);
				}
				break;
			}
		}
	}
	//UpdateServerWindowTitle();
}


void AXivSimGameState::NotifyPlayerTakeDamage(class AXivSimCharacter* PlayerPtr, float DamageCount, const TSubclassOf<class UXivSimDamageType> DamageTypeClass, AActor* DamageCauser)
{
	FPlayerTakeDamageInfo NewInfo;
	NewInfo.PlayerPtr = PlayerPtr;
	NewInfo.DamageCauser = DamageCauser;
	NewInfo.DamageTypeClass = DamageTypeClass;
	NewInfo.DamageAmount = DamageCount;

	//BroadcastPlayerDamageInfo(NewInfo);
}

void AXivSimGameState::BroadcastPlayerDamageInfo_Implementation(FPlayerTakeDamageInfo DamageInfo)
{

}

void AXivSimGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AXivSimGameState, PatternItemContainer);
	DOREPLIFETIME(AXivSimGameState, bIgnorePlayerDamage);
	DOREPLIFETIME(AXivSimGameState, bIsTrainingMode);
}

bool AXivSimGameState::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags); 
	UActorChannel::SetCurrentSubObjectOwner(this);
	for (FPatternItem PatternItem : PatternItemContainer.PatternList)
	{
		if (IsValid(PatternItem.Pattern))
		{
			WroteSomething |= Channel->ReplicateSubobject(PatternItem.Pattern, *Bunch, *RepFlags);
		}
	}
	return WroteSomething;
}

void AXivSimGameState::AddPattern(class UXivSimPatternBase* InPattern)
{
	if (HasAuthority())
	{
		if (InPattern)
		{
			InPattern->OnActivated();
			FPatternItem PatternItem;
			PatternItem.Pattern = InPattern;
			PatternItemContainer.PatternList.Add(PatternItem);
			//AddReplicatedSubObject(InPattern);
			PatternItemContainer.MarkArrayDirty();
			ForceNetUpdate();
		}
	}
}

void AXivSimGameState::RemovePattern(class UXivSimPatternBase* InPattern)
{
	if (HasAuthority())
	{
		if (InPattern)
		{
			for (int32 i = PatternItemContainer.PatternList.Num() - 1; i >= 0; --i)
			{
				if (PatternItemContainer.PatternList[i].Pattern == InPattern)
				{
					InPattern->OnDeactivated();
					//RemoveReplicatedSubObject(InPattern);
					PatternItemContainer.PatternList.RemoveAt(i);
					PatternItemContainer.MarkArrayDirty();
					break;
				}
			}
		}
	}
}

void AXivSimGameState::SetIgnorePlayerDamage(bool NewValue)
{
	if(bIgnorePlayerDamage != NewValue)
	{
		bIgnorePlayerDamage = NewValue;
		OnSetIgnorePlayerDamage(NewValue);
		OnRep_IgnorePlayerDamage();
	}
}

void AXivSimGameState::SetTrainingMode(bool NewValue)
{
	if(bIsTrainingMode != NewValue)
	{
		bIsTrainingMode = NewValue;
		OnSetTrainingMode(NewValue);
		OnRep_TrainingMode();
	}
}

void AXivSimGameState::UpdateServerWindowTitle()
{
	if(IsNetMode(NM_DedicatedServer))
	{
		FString NewTitle = FString::Printf(TEXT("XivSimServer %s %dP"), *GetWorld()->GetAddressURL(), PlayerArray.Num());
		UKismetSystemLibrary::SetWindowTitle(FText::FromString(NewTitle));
	}
}

void AXivSimGameState::OnRep_IgnorePlayerDamage()
{
	OnGodModeChanged.Broadcast(IsIgnorePlayerDamage());
}

void AXivSimGameState::OnRep_TrainingMode()
{
	OnTrainingModeChanged.Broadcast(IsTrainingMode());
}

void FPatternItem::PreReplicatedRemove(const struct FPatternItemContainer& InArraySerializer)
{
	if (Pattern)
	{
		Pattern->OnDeactivated();
	}
}

void FPatternItem::PostReplicatedAdd(const struct FPatternItemContainer& InArraySerializer)
{
	if (Pattern)
	{
		Pattern->OnActivated();
	}
}

void FPatternItem::PostReplicatedChange(const struct FPatternItemContainer& InArraySerializer)
{
	if (Pattern)
	{
		Pattern->OnActivated();
	}
}

void FPatternItemContainer::Clear()
{
	for (auto Iter = PatternList.CreateIterator(); Iter; ++Iter)
	{
		if (Iter->Pattern)
		{
			Iter->Pattern->OnDeactivated();
		}
		Iter.RemoveCurrent();
	}
}
