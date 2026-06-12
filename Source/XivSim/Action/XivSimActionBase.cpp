// Copyright@Homura Akane@Chocobo


#include "XivSimActionBase.h"
#include "XivSimCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

void UXivSimActionBase::SetActionData(const FXivSimActionData& InData)
{
	ActionData = InData;
}

void UXivSimActionBase::ActivateAction(AXivSimCharacter* SourceCharacter, AXivSimCharacterBase* TargetCharacter)
{
	if(SourceCharacter)
	{
		if (!SourceCharacter->HasAuthority())
		{
			UCharacterMovementComponent* CharMoveComp = Cast<UCharacterMovementComponent>(SourceCharacter->GetMovementComponent());

			if (CharMoveComp)
			{
				CharMoveComp->FlushServerMoves();
			}
		}
		
		if(ActionData.bNeedTarget && TargetCharacter)
		{
			const FVector TargetDir = TargetCharacter->GetActorLocation() - SourceCharacter->GetActorLocation();
			SourceCharacter->SetActorRotation(FRotationMatrix::MakeFromZX(FVector::UpVector, TargetDir).Rotator());
		}

		if(!SourceCharacter->IsNetMode(NM_DedicatedServer))
		{
			if(ActionData.ActionAnimMontage != nullptr)
			{
				SourceCharacter->PlayAnimMontage(ActionData.ActionAnimMontage);
			}
		}
	}
	ActivateAction_BP(SourceCharacter, TargetCharacter);
}

void UXivSimActionBase::CancelAction(AXivSimCharacter* SourceCharacter)
{
	if(SourceCharacter)
	{
		if(!SourceCharacter->IsNetMode(NM_DedicatedServer))
		{
			if(ActionData.ActionAnimMontage != nullptr)
			{
				SourceCharacter->StopAnimMontage(ActionData.ActionAnimMontage);
			}
		}
	}
}

void UXivSimActionBase::EndAction(AXivSimCharacter* SourceCharacter)
{
	if(SourceCharacter)
	{
		if(!SourceCharacter->IsNetMode(NM_DedicatedServer))
		{
			if(ActionData.ActionAnimMontage != nullptr)
			{
				SourceCharacter->PlayAnimMontage(ActionData.ActionAnimMontage, 1.f, FName("End"));
			}
		}
	}
}
