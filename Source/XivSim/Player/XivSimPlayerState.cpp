#include "XivSimPlayerState.h"

#include "XivSimCharacter.h"
#include "XivSimPlayerController.h"
#include "XivSimPlayerManager.h"
#include "Net/UnrealNetwork.h"

AXivSimPlayerState::AXivSimPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUseCustomPlayerNames = true;
	SetXivSimPlayerCustomName("FFXIV Player");
	CurrentRolePlayType = EXivSimRolePlayType::RolePlay_All;
	CurrentClassType = EXivSimClassType::Class_None;
	CurrentPositionType = EXivSimPositionType::MT;
}

void AXivSimPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AXivSimPlayerState, XivSimCustomPlayerName);
	DOREPLIFETIME(AXivSimPlayerState, CurrentRolePlayType);
	DOREPLIFETIME(AXivSimPlayerState, CurrentClassType);
	DOREPLIFETIME(AXivSimPlayerState, CurrentMarkerType);
	DOREPLIFETIME(AXivSimPlayerState, CurrentPositionType);
	DOREPLIFETIME(AXivSimPlayerState, CurrentSelectedCharacter);
}

FString AXivSimPlayerState::GetPlayerNameCustom() const
{
	return XivSimCustomPlayerName;
}

void AXivSimPlayerState::SetXivSimPlayerCustomName(const FString& NewName)
{
	XivSimCustomPlayerName = NewName;
	ForceNetUpdate();
	OnRep_XivSimCustomPlayerName();
}

void AXivSimPlayerState::SetCurrentRolePlayType(EXivSimRolePlayType NewType)
{
	if(CurrentRolePlayType != NewType)
	{
		if (UXivSimPlayerManager* PlayerManager = UXivSimPlayerManager::Get(this))
		{
			PlayerManager->RemovePlayerState(this);

			CurrentRolePlayType = NewType;
			if(AXivSimCharacter* PlayerPawn = Cast<AXivSimCharacter>(GetPawn()))
			{
				PlayerPawn->SetRolePlayType(NewType);
			}
			
			PlayerManager->AddPlayerState(this);
		
			OnRep_CurrentRolePlayType();
		}
	}
}

void AXivSimPlayerState::SetCurrentClassType(EXivSimClassType NewType)
{
	if(CurrentClassType != NewType)
	{
		if (UXivSimPlayerManager* PlayerManager = UXivSimPlayerManager::Get(this))
		{
			CurrentClassType = NewType;
			if(AXivSimCharacter* PlayerPawn = Cast<AXivSimCharacter>(GetPawn()))
			{
				PlayerPawn->SetClassType(NewType);
			}
			const EXivSimRolePlayType NewRoleType = PlayerManager->GetClassRolePlayType(NewType);
			SetCurrentRolePlayType(NewRoleType);
			OnRep_CurrentClassType();
		}
	}
}

void AXivSimPlayerState::SetCurrentPositionType(EXivSimPositionType NewPos)
{
	if(CurrentPositionType != NewPos)
	{
		CurrentPositionType = NewPos;
		OnRep_CurrentPositionType();
	}
}

void AXivSimPlayerState::SetCurrentMarkerType(EXivSimMarkerType NewType)
{
	if(CurrentMarkerType != NewType)
	{
		CurrentMarkerType = NewType;
		OnRep_CurrentMarkerType();
	}
}

void AXivSimPlayerState::SetCurrentSelectedCharacter(AXivSimCharacterBase* NewCharacter)
{
	if(CurrentSelectedCharacter != NewCharacter)
	{
		if(CurrentSelectedCharacter)
		{
			CurrentSelectedCharacter->OnCanBeTargetedStateChanged.RemoveDynamic(this, &AXivSimPlayerState::OnTargetCharacterCanBeSelectedStateChanged);
		}
		CurrentSelectedCharacter = NewCharacter;
		if(CurrentSelectedCharacter)
		{
			CurrentSelectedCharacter->OnCanBeTargetedStateChanged.AddUniqueDynamic(this, &AXivSimPlayerState::OnTargetCharacterCanBeSelectedStateChanged);
		}
		OnRep_CurrentSelectedCharacter();
	}
}

void AXivSimPlayerState::SetPlayerNameBP(const FString& NewName)
{
	ServerSetPlayerName(NewName);
}

void AXivSimPlayerState::ServerSetPlayerName_Implementation(const FString& NewName)
{
	SetXivSimPlayerCustomName(NewName);
}

void AXivSimPlayerState::ServerSetPlayerClassType_Implementation(EXivSimClassType NewType)
{
	SetCurrentClassType(NewType);
}

void AXivSimPlayerState::ServerSetCurrentMarkerType_Implementation(EXivSimMarkerType NewType)
{
	if (UXivSimPlayerManager* PlayerManager = UXivSimPlayerManager::Get(this))
	{
		PlayerManager->SetPlayerMarkerType(this, NewType);
	}
}

void AXivSimPlayerState::ServerSetPlayerPositionType_Implementation(EXivSimPositionType NewType)
{
	SetCurrentPositionType(NewType);
}

void AXivSimPlayerState::ServerClearAllPlayerMarker_Implementation()
{
	if (UXivSimPlayerManager* PlayerManager = UXivSimPlayerManager::Get(this))
	{
		PlayerManager->ClearAllPlayerMarker();
	}
}


void AXivSimPlayerState::ServerSetCurrentSelectedCharacter_Implementation(AXivSimCharacterBase* NewSelectedCharacter)
{
	SetCurrentSelectedCharacter(NewSelectedCharacter);
}

void AXivSimPlayerState::OnRep_CurrentRolePlayType()
{
	if(!GetWorld()->IsNetMode(NM_DedicatedServer))
	{
		OnRolePlayTypeChange.Broadcast(CurrentRolePlayType);
	}
}

void AXivSimPlayerState::OnRep_CurrentClassType()
{
	if(!GetWorld()->IsNetMode(NM_DedicatedServer))
	{
		OnClassTypeChange.Broadcast(CurrentClassType);
	}
}

void AXivSimPlayerState::OnRep_CurrentPositionType()
{
	if(!GetWorld()->IsNetMode(NM_DedicatedServer))
	{
		OnPositionTypeChange.Broadcast(CurrentPositionType);
	}
}

void AXivSimPlayerState::OnRep_CurrentMarkerType()
{
	if (!GetWorld()->IsNetMode(NM_DedicatedServer))
	{
		OnMarkerTypeChange.Broadcast(CurrentMarkerType);
	}
}

void AXivSimPlayerState::OnRep_CurrentSelectedCharacter()
{
	if(!GetWorld()->IsNetMode(NM_DedicatedServer))
	{
		if(GetMyPlayerController())
		{
			GetMyPlayerController()->SelectCharacterInWorld(CurrentSelectedCharacter);
		}

		OnSelectedCharacterChange.Broadcast(CurrentSelectedCharacter);
	}
}

void AXivSimPlayerState::OnRep_XivSimCustomPlayerName()
{
	OnPlayerNameChange.Broadcast(GetPlayerName());
}

AXivSimPlayerController* AXivSimPlayerState::GetMyPlayerController()
{
	return Cast<AXivSimPlayerController>(GetPlayerController());
}

void AXivSimPlayerState::OnTargetCharacterCanBeSelectedStateChanged(AXivSimCharacterBase* TargetChar, bool bNewState)
{
	if(TargetChar == CurrentSelectedCharacter)
	{
		if(!bNewState)
		{
			SetCurrentSelectedCharacter(nullptr);
		}
	}
}
