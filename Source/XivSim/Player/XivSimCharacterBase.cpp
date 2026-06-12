#include "XivSimCharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"

AXivSimCharacterBase::AXivSimCharacterBase(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void AXivSimCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AXivSimCharacterBase, TeamID);
	DOREPLIFETIME(AXivSimCharacterBase, CharacterName);
	DOREPLIFETIME(AXivSimCharacterBase, bCanBeTargeted);
	DOREPLIFETIME(AXivSimCharacterBase, bEntranced);
}

void AXivSimCharacterBase::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if(TeamID != NewTeamID)
	{
		TeamID = NewTeamID;
	}
}

FGenericTeamId AXivSimCharacterBase::GetGenericTeamId() const
{
	return TeamID;
}

FString AXivSimCharacterBase::GetCharacterName() const
{
	return CharacterName;
}

float AXivSimCharacterBase::GetCharacterHitRadius() const
{
	return CharacterHitRadius;
}

void AXivSimCharacterBase::SetCanBeTargeted(bool bNewValue)
{
	if(bCanBeTargeted != bNewValue)
	{
		bCanBeTargeted = bNewValue;
	}
}

void AXivSimCharacterBase::SetEntranced(bool bNewCan)
{
	if(bEntranced != bNewCan)
	{
		bEntranced = bNewCan;
		if(bCanBeTargeted)
		{
			OnCanBeTargetedStateChanged.Broadcast(this, CanBeTargeted());
		}
	}
}

const bool AXivSimCharacterBase::CanBeTargeted() const
{
	return bCanBeTargeted && bEntranced;
}

void AXivSimCharacterBase::CharacterSelected()
{
	if(!bLocalSelected)
	{
		bLocalSelected = true;
		OnCharacterSelected();
	}
}

void AXivSimCharacterBase::CharacterUnselected()
{
	bLocalSelected = false;
	OnCharacterUnselected();
}

FVector AXivSimCharacterBase::GetTargetRingRelativeLocation() const
{
	return FVector::Zero();
}

FVector AXivSimCharacterBase::GetTargetRingDecalSize() const
{
	float Radius = GetCharacterHitRadius();
	return FVector(200.f, Radius, Radius);
}

void AXivSimCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if(!CanBeTargeted())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
}

void AXivSimCharacterBase::OnRep_Entranced()
{
	OnEntrancedStateChanged();
}
