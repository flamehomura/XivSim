#include "XivSimBuffAttributeModifier.h"
#include "XivSimCharacter.h"

UXivSimBuffAttributeModifier::UXivSimBuffAttributeModifier()
{
	bAllowSimulate = false;
}

void UXivSimBuffAttributeModifier::OnActivated(bool bClientSimulated)
{
	Super::OnActivated();
	if (bAllowSimulate || HasAuthority())
	{
		if (AXivSimCharacter* PlayerOwner = GetPlayerOwner())
		{
			PlayerOwner->AddAttributeModifier(AttributeType, ModifierValue);
		}
	}
}

void UXivSimBuffAttributeModifier::OnDeactivated(bool bClientSimulated)
{
	if (bAllowSimulate || HasAuthority())
	{
		if (AXivSimCharacter* PlayerOwner = GetPlayerOwner())
		{
			PlayerOwner->AddAttributeModifier(AttributeType, 1.f / ModifierValue);
		}
	}
	Super::OnDeactivated();
}

