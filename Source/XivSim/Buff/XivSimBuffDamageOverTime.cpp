// Copyright@Homura Akane@Chocobo


#include "XivSimBuffDamageOverTime.h"

#include "XivSimCharacter.h"
#include "Engine/DamageEvents.h"
#include "XivSimDamageType.h"
#include "XivSimGameState.h"

UXivSimBuffDamageOverTime::UXivSimBuffDamageOverTime()
{
}

void UXivSimBuffDamageOverTime::TickBuff(float DeltaSeconds)
{
	Super::TickBuff(DeltaSeconds);
	if (HasAuthority())
	{
		DamageTimer += DeltaSeconds;
		if(DamageTimer > DamageInterval)
		{
			DamageTimer -= DamageInterval;
			
			if (AXivSimCharacter* PlayerOwner = GetPlayerOwner())
			{
				PlayerOwner->TakeDamage(
							DamageAmount,
							FDamageEvent(DamageTypeClass),
							nullptr,
							GetOwnerActor() == nullptr ? GetWorld()->GetGameState() : GetOwnerActor());
			}
		}
	}
}

void UXivSimBuffDamageOverTime::OnActivated(bool bClientSimulated)
{
	Super::OnActivated(bClientSimulated);
	if (HasAuthority())
	{
		DamageTimer = 0.f;
	}
}

void UXivSimBuffDamageOverTime::OnDeactivated(bool bClientSimulated)
{
	if (HasAuthority())
	{
		DamageTimer = 0.f;
	}
	Super::OnDeactivated(bClientSimulated);
}
