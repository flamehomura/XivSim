// Copyright@Homura Akane@Chocobo


#include "XivSimBuffChain.h"

#include "BuffComponent.h"

UXivSimBuffChain::UXivSimBuffChain()
{
}

void UXivSimBuffChain::TickBuff(float DeltaSeconds)
{
	Super::TickBuff(DeltaSeconds);
}

void UXivSimBuffChain::OnTimeOver()
{
	Super::OnTimeOver();

	if(HasAuthority() && bIsLeader)
	{
		if(TimeOverBuffClass)
		{
			UBuffComponent* PairedBuffComponent = PairedBuff->GetOwnerBuffComponent();
			if (OwnerBuffComponent && PairedBuffComponent)
			{
				UXivSimBuffPaired* PairedBuffLeader = Cast<UXivSimBuffPaired>(OwnerBuffComponent->AddBuffByClass(TimeOverBuffClass, GetOuter()));
				if (PairedBuffLeader)
				{
					PairedBuffLeader->SetIsLeader(true);
					UXivSimBuffPaired* PairedBuffFollower = Cast<UXivSimBuffPaired>(PairedBuffComponent->AddBuffByClass(TimeOverBuffClass, GetOuter()));
					if (PairedBuffFollower)
					{
						PairedBuffFollower->SetIsLeader(false);
						PairedBuffLeader->SetPairedBuff(PairedBuffFollower);
					}
				}
			}
		}
	}
}
