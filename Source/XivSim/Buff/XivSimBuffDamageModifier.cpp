#include "XivSimBuffDamageModifier.h"
#include "XivSimCharacter.h"

UXivSimBuffDamageModifier::UXivSimBuffDamageModifier()
{

}

void UXivSimBuffDamageModifier::OnActivated(bool bClientSimulated)
{
	Super::OnActivated();
	if (HasAuthority())
	{
		if (AXivSimCharacter* PlayerOwner = GetPlayerOwner())
		{
			PlayerOwner->AddDamageModifier(DamageType, ModifierValue);
		}
	}
}

void UXivSimBuffDamageModifier::OnDeactivated(bool bClientSimulated)
{
	if (HasAuthority())
	{
		if (AXivSimCharacter* PlayerOwner = GetPlayerOwner())
		{
			PlayerOwner->AddDamageModifier(DamageType, 1.f / ModifierValue);
		}
	}
	Super::OnDeactivated();
}

