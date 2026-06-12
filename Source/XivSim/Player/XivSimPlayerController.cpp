#include "XivSimPlayerController.h"

#include "EnhancedInputComponent.h"
#include "XivSimCharacterBase.h"
#include "XivSimPlayerState.h"
#include "XivSimCheatManager.h"
#include "XivSimLocalPlayer.h"
#include "Components/DecalComponent.h"
#include "GameFramework/PlayerInput.h"
#include "EnhancedInputSubsystems.h"
#include "XivSimCharacter.h"
#include "XivSimGameState.h"
#include "XivSimGameUserSettings.h"
#include "Kismet/GameplayStatics.h"

AXivSimPlayerController::AXivSimPlayerController(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	CheatClass = UXivSimCheatManager::StaticClass();
	bSelectionInterrupted = false;
	CurrentSelectedCharacter = nullptr;
}

void AXivSimPlayerController::ServerExecRPC_Implementation(const FString& Msg)
{
	Super::ServerExecRPC_Implementation(Msg);
}

void AXivSimPlayerController::TrySetPlayerName(FString NewName)
{
	if (IsLocalController())
	{
		if (AXivSimPlayerState* PS = GetPlayerState<AXivSimPlayerState>())
		{
			PS->ServerSetPlayerName(NewName);
		}

		if (UXivSimLocalPlayer* LP = Cast<UXivSimLocalPlayer>(GetLocalPlayer()))
		{
			LP->SetPlayerNickname(NewName);
		}

		if (UXivSimGameUserSettings* GUS = UXivSimGameUserSettings::GetXivSimGameUserSettings())
		{
			GUS->SetLocalPlayerName(NewName);
		}
	}
}

void AXivSimPlayerController::TrySetClassType(EXivSimClassType NewType)
{
	if (IsLocalController())
	{
		if (AXivSimPlayerState* PS = GetPlayerState<AXivSimPlayerState>())
		{
			PS->ServerSetPlayerClassType(NewType);
		}

		if (UXivSimGameUserSettings* GUS = UXivSimGameUserSettings::GetXivSimGameUserSettings())
		{
			GUS->SetLocalPlayerClassType(NewType);
		}
	}
}

void AXivSimPlayerController::TrySetPositionType(EXivSimPositionType NewType)
{
	if (IsLocalController())
	{
		if (AXivSimPlayerState* PS = GetPlayerState<AXivSimPlayerState>())
		{
			PS->ServerSetPlayerPositionType(NewType);
		}

		if (UXivSimGameUserSettings* GUS = UXivSimGameUserSettings::GetXivSimGameUserSettings())
		{
			GUS->SetLocalPlayerPositionType(NewType);
		}
	}
}

bool AXivSimPlayerController::InputKey(const FInputKeyEventArgs& Params)
{
	if(Params.Key == EKeys::LeftMouseButton || Params.Key == EKeys::RightMouseButton)
	{
		if(Params.Event == EInputEvent::IE_Pressed)
		{
			bSelectionInterrupted = false;
			LastSelectionHitTime = GetWorld()->GetTimeSeconds();
		}
		
		if(Params.Event == EInputEvent::IE_Released)
		{
			if(!bSelectionInterrupted)
			{
				TrySelectWorldActor();
			}
		}
	}

	if(Params.Key == EKeys::MouseX || Params.Key == EKeys::MouseY || Params.Key == EKeys::Mouse2D)
	{
		if(GetWorld()->GetTimeSeconds() - LastSelectionHitTime > SelectionInterruptTimeThreshold)
		{
			bSelectionInterrupted = true;
		}
	}
	
	return Super::InputKey(Params);
}

bool AXivSimPlayerController::TrySelectWorldActor()
{
	FVector WorldLocation, WorldDirection;
	if(DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		FHitResult Hit;
		UWorld* World = GetWorld();
		check(World);
		if (World->LineTraceSingleByObjectType(Hit, WorldLocation, WorldLocation + WorldDirection * WorldActorTraceLength, FCollisionObjectQueryParams(ECC_Pawn)))
		{
			if(AXivSimCharacterBase* HitCharacter = Cast<AXivSimCharacterBase>(Hit.GetActor()))
			{
				if(HitCharacter->CanBeTargeted())
				{
					CachedTabTargetingList.Empty();
					TrySelectCharacterInWorld(HitCharacter);
					return true;
				}
			}
		}

		if(!bDisableSelectHitNULL)
		{
			CachedTabTargetingList.Empty();
			TrySelectCharacterInWorld(nullptr);
		}
		return false;
	}

	return false;
}

bool AXivSimPlayerController::TrySelectCharacterInWorld(AXivSimCharacterBase* NewCharacter)
{
	if(IsLocalController())
	{
		if(NewCharacter != CurrentSelectedCharacter)
		{
			if (AXivSimPlayerState* PS = GetPlayerState<AXivSimPlayerState>())
			{
				PS->ServerSetCurrentSelectedCharacter(NewCharacter);
				return true;
			}
		}
	}

	return false;
}

void AXivSimPlayerController::SelectCharacterInWorld(class AXivSimCharacterBase* NewCharacter)
{
	if(!IsNetMode(NM_DedicatedServer))
	{
		if(CurrentSelectedCharacter)
		{
			CurrentSelectedCharacter->CharacterUnselected();
		}
	
		if(NewCharacter)
		{
			NewCharacter->CharacterSelected();
		}
		AttachTargetRingTo(NewCharacter);
		CurrentSelectedCharacter = NewCharacter;
	}
}

AXivSimCharacterBase* AXivSimPlayerController::GetCurrentSelectedCharacter() const
{
	return CurrentSelectedCharacter;
}

void AXivSimPlayerController::TrySetPartyGodMode(bool bGod)
{
	if(IsLocalController())
	{
		ServerSetPartyGodMode(bGod);
	}
}

void AXivSimPlayerController::ServerSetPartyGodMode_Implementation(bool bGod)
{
	if(AXivSimGameState* GS = GetWorld()->GetGameState<AXivSimGameState>())
	{
		GS->SetIgnorePlayerDamage(bGod);
	}
}

void AXivSimPlayerController::TrySetTrainingMode(bool bGod)
{
	if(IsLocalController())
	{
		ServerSetTrainingMode(bGod);
	}
}

void AXivSimPlayerController::ServerSetTrainingMode_Implementation(bool bGod)
{
	if(AXivSimGameState* GS = GetWorld()->GetGameState<AXivSimGameState>())
	{
		GS->SetTrainingMode(bGod);
	}
}

void AXivSimPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
	
	if(IsLocalController())
	{
		if(AXivSimCharacter* PlayerPawn = Cast<AXivSimCharacter>(GetPawn()))
		{
			if(UXivSimGameUserSettings* GUS = UXivSimGameUserSettings::GetXivSimGameUserSettings())
			{
				PlayerPawn->ServerSetInputMode(GUS->GetUseStandardInputMode());
			}
		}
		
		if(UXivSimGameUserSettings* GUS = UXivSimGameUserSettings::GetXivSimGameUserSettings())
		{
			TrySetPlayerName(GUS->GetLocalPlayerName());
			TrySetClassType(GUS->GetLocalPlayerClassType());
			TrySetPositionType(GUS->GetLocalPlayerPositionType());
		}
	}

	if(!IsNetMode(NM_DedicatedServer))
	{
		CreateTargetRingDecal();
	}
}

void AXivSimPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(TargetRingDecalComponent))
	{
		TargetRingDecalComponent->DestroyComponent();
		TargetRingDecalComponent = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}

void AXivSimPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(TabTargetingAction, ETriggerEvent::Completed, this, &AXivSimPlayerController::TabTargeting);
	}
}

void AXivSimPlayerController::TabTargeting()
{
	if(IsLocalController() && GetPawn())
	{
		TArray<AActor*> AllCharacters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AXivSimCharacterBase::StaticClass(), AllCharacters);
		for(AActor* IterChar : AllCharacters)
		{
			if(AXivSimCharacterBase* XivSimChar = Cast<AXivSimCharacterBase>(IterChar))
			{
				if(XivSimChar == CurrentSelectedCharacter)
				{
					continue;
				}

				if(!XivSimChar->CanBeTargeted())
				{
					continue;
				}
				
				if(GetWorld()->TimeSince(XivSimChar->GetLastRenderTime()) < 0.1f)
				{
					if(FGenericTeamId::GetAttitude(GetPawn(), XivSimChar) == ETeamAttitude::Type::Hostile)
					{
						if(!CachedTabTargetingList.Contains(XivSimChar))
						{
							if(TrySelectCharacterInWorld(XivSimChar))
							{
								CachedTabTargetingList.Add(XivSimChar);
								TempCachedTabTargetingList.Empty();
								return;
							}
						}
						else
						{
							TempCachedTabTargetingList.Add(XivSimChar);
						}
					}
				}
			}
		}

		CachedTabTargetingList.Empty();
		if(TempCachedTabTargetingList.Num() > 0)
		{
			for(AXivSimCharacterBase* XivSimChar : TempCachedTabTargetingList)
			{
				if(TrySelectCharacterInWorld(XivSimChar))
				{
					CachedTabTargetingList.Add(XivSimChar);
					break;
				}
			}
			
			TempCachedTabTargetingList.Empty();
		}
	}
}

void AXivSimPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	if(IsLocalController())
	{
		if(AXivSimCharacter* PlayerPawn = Cast<AXivSimCharacter>(GetPawn()))
		{
			if(UXivSimGameUserSettings* GUS = UXivSimGameUserSettings::GetXivSimGameUserSettings())
			{
				PlayerPawn->ServerSetInputMode(GUS->GetUseStandardInputMode());
			}
		}
	}
}

void AXivSimPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if(UXivSimGameUserSettings* GUS = UXivSimGameUserSettings::GetXivSimGameUserSettings())
	{
		TrySetPlayerName(GUS->GetLocalPlayerName());
		TrySetClassType(GUS->GetLocalPlayerClassType());
		TrySetPositionType(GUS->GetLocalPlayerPositionType());
	}

	CreateTargetRingDecal();

	OnPlayerStateChange.Broadcast(Cast<AXivSimPlayerState>(PlayerState));
}

void AXivSimPlayerController::CreateTargetRingDecal()
{
	if(!IsNetMode(NM_DedicatedServer))
	{
		if(!IsValid(TargetRingDecalComponent))
		{
			TargetRingDecalComponent = NewObject<UDecalComponent>(GetWorld());
			if(TargetRingDecalComponent)
			{
				TargetRingDecalComponent->bAllowAnyoneToDestroyMe = true;
				TargetRingDecalComponent->SetDecalMaterial(TargetRingDecalMaterial);
				TargetRingDecalComponent->CreateDynamicMaterialInstance();
				TargetRingDecalComponent->DecalSize = FVector(100.f);
				TargetRingDecalComponent->SetUsingAbsoluteScale(true);
				TargetRingDecalComponent->RegisterComponentWithWorld(GetWorld());
				TargetRingDecalComponent->SetVisibility(false);
			}
		}
	}
}

void AXivSimPlayerController::AttachTargetRingTo(AXivSimCharacterBase* TargetCharacter)
{
	if(!IsNetMode(NM_DedicatedServer))
	{
		if(IsValid(TargetRingDecalComponent))
		{
			if(TargetCharacter && TargetCharacter->GetRootComponent())
			{
				TargetRingDecalComponent->AttachToComponent(
					TargetCharacter->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				TargetRingDecalComponent->SetRelativeLocation(TargetCharacter->GetTargetRingRelativeLocation());
				TargetRingDecalComponent->SetWorldRotation(FRotationMatrix::MakeFromXY(FVector(0, 0, -1), TargetCharacter->GetActorRotation().Vector() * -1).Rotator());
				TargetRingDecalComponent->DecalSize = TargetCharacter->GetTargetRingDecalSize();
				TargetRingDecalComponent->SetVisibility(true);

				if(AXivSimCharacterBase* MyChar = Cast<AXivSimCharacterBase>(GetPawn()))
				{
					TargetRingDecalComponent->SetDecalColor(FGenericTeamId::GetAttitude(MyChar, TargetCharacter) == ETeamAttitude::Type::Hostile ?
						FLinearColor::Red : FLinearColor::Green);
				}
			}
			else
			{
				TargetRingDecalComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
				TargetRingDecalComponent->SetVisibility(false);
			}
		}
	}
}

