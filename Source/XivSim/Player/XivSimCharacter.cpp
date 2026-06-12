// Copyright Epic Games, Inc. All Rights Reserved.

#include "XivSimCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/WidgetComponent.h"
#include "XivSimPlayerMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BuffComponent.h"
#include "XivSimGameState.h"
#include "Engine/DamageEvents.h"
#include "XivSimDamageType.h"
#include "XivSimGameUserSettings.h"
#include "XivSimPlayerState.h"
#include "XivSimPlayerManager.h"
#include "XivSimMarkerWidget.h"
#include "XivSimActionComponent.h"

//////////////////////////////////////////////////////////////////////////
// AXivSimCharacter

AXivSimCharacter::AXivSimCharacter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UXivSimPlayerMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	MarkerWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("MarkerWidget"));
	MarkerWidget->SetupAttachment(RootComponent);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	BuffComponent = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	BuffComponent->SetIsReplicated(true);

	ActionComponent = CreateDefaultSubobject<UXivSimActionComponent>(TEXT("ActionComponent"));
	ActionComponent->SetIsReplicated(true);

	GetMesh()->bReceivesDecals = false;

	RolePlayType = EXivSimRolePlayType::RolePlay_Tank;
	CurrentState = EXivSimCharacterState::State_Alive;
}

float AXivSimCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	ApplyDamageModifier(ActualDamage, DamageEvent.DamageTypeClass);
	ApplyDamageBuff(DamageCauser, DamageEvent.DamageTypeClass);

	if (AXivSimGameState* GameState = GetWorld()->GetGameState<AXivSimGameState>())
	{
		if (DamageEvent.DamageTypeClass->IsChildOf(UXivSimDamageType::StaticClass()))
		{
			TSubclassOf<UXivSimDamageType> SimDT = *DamageEvent.DamageTypeClass;
			GameState->NotifyPlayerTakeDamage(this, ActualDamage, SimDT, DamageCauser);
		}
	}

	int32 LastHealth = CurrentHealth;
	SetHealth(CurrentHealth - (uint32)ActualDamage);
	if(LastHealth < ActualDamage && CurrentHealth > 0)
	{
		OnPlayerUndying.Broadcast();
	}
	if (LastHealth != CurrentHealth)
	{
		if (CurrentHealth <= 0)
		{
			SetCurrentCharacterState(EXivSimCharacterState::State_Dead);
		}
	}

	return ActualDamage;
}

bool AXivSimCharacter::ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	if (AXivSimGameState* GameState = GetWorld()->GetGameState<AXivSimGameState>())
	{
		if (IsInGodMode())
		{
			return false;
		}
	}

	return Super::ShouldTakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void AXivSimCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	//Super::FellOutOfWorld(dmgType);
	if (HasAuthority())
	{
		SetActorLocation(FVector(0, 0, 500));
		TakeDamage(999999, FDamageEvent(dmgType.GetClass()), nullptr, this);
		SetCurrentCharacterState(EXivSimCharacterState::State_Dead);
	}
}

void AXivSimCharacter::SetCurrentCharacterState(EXivSimCharacterState NewState)
{
	if (CurrentState != NewState)
	{
		CurrentState = NewState;
		NotifyCurrentCharacterStateChanged();
	}
}

bool AXivSimCharacter::IsAlive() const
{
	return CurrentState == EXivSimCharacterState::State_Alive;
}

bool AXivSimCharacter::CheckStillInWorld()
{
	UXivSimPlayerManager* PlayerManager = UXivSimPlayerManager::Get(this);
	if (PlayerManager)
	{
		if (!PlayerManager->CheckPlayerInArena(this))
		{
			TakeDamage(9999999, FDamageEvent(PlayerManager->GetOutOfArenaDamageType()), nullptr, this);
			return false;
		}
	}

	return Super::CheckStillInWorld();
}

void AXivSimCharacter::SetStun(bool bNewStun)
{
	bCurrentStunning = bNewStun;
}

bool AXivSimCharacter::IsStunning() const
{
	return bCurrentStunning;
}

void AXivSimCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (HasAuthority())
	{
		CurrentHealth = MaxHealth;
	}

	if (GetWorld()->IsNetMode(NM_DedicatedServer))
	{
		MarkerWidget->DestroyComponent();
		MarkerWidget = nullptr;
	}
	else
	{
		if (AXivSimPlayerState* XivPS = Cast<AXivSimPlayerState>(GetPlayerState()))
		{
			if (MarkerWidget)
			{
				if (UXivSimMarkerWidget* Widget = Cast<UXivSimMarkerWidget>(MarkerWidget->GetUserWidgetObject()))
				{
					Widget->SetPlayerState(XivPS);
				}
			}
		}
	}
}

void AXivSimCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TickAutoMoving(DeltaSeconds);
}

void AXivSimCharacter::OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState)
{
	Super::OnPlayerStateChanged(NewPlayerState, OldPlayerState);

	if(HasAuthority())
	{
		if(AXivSimPlayerState* PS = Cast<AXivSimPlayerState>(NewPlayerState))
		{
			SetClassType(PS->GetCurrentClassType());
		}
	}
}

void AXivSimCharacter::NotifyCurrentCharacterStateChanged()
{
	switch (CurrentState)
	{
	case EXivSimCharacterState::State_Dead:
		OnDead();
		break;

	default:
		break;
	}
}

void AXivSimCharacter::OnRep_RolePlayType()
{
	NotifyRolePlayTypeChanged();
}

void AXivSimCharacter::OnRep_ClassType()
{
	NotifyClassTypeChanged();
}

void AXivSimCharacter::OnRep_CurrentState()
{
	NotifyCurrentCharacterStateChanged();
}

void AXivSimCharacter::OnRep_CurrentHealth()
{
	OnPlayerHealthChange.Broadcast(CurrentHealth, GetMaxHealth());
}

void AXivSimCharacter::OnRep_StandardInputMode()
{
	if (bStandardInputMode)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

void AXivSimCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (!GetWorld()->IsNetMode(NM_DedicatedServer))
	{
		if (AXivSimPlayerState* XivPS = Cast<AXivSimPlayerState>(GetPlayerState()))
		{
			if (MarkerWidget)
			{
				if (UXivSimMarkerWidget* Widget = Cast<UXivSimMarkerWidget>(MarkerWidget->GetUserWidgetObject()))
				{
					Widget->SetPlayerState(XivPS);
				}
			}
		}
	}
}

void AXivSimCharacter::OnDead()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
	}

	GetBuffComponent()->ClearBuffs();
	SetActorHiddenInGame(true);

	OnPlayerDead.Broadcast(this);
}

void AXivSimCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AXivSimCharacter, bStandardInputMode);
	DOREPLIFETIME(AXivSimCharacter, RolePlayType);
	DOREPLIFETIME(AXivSimCharacter, ClassType);
	DOREPLIFETIME(AXivSimCharacter, CurrentState);
	DOREPLIFETIME(AXivSimCharacter, MaxHealth);
	DOREPLIFETIME(AXivSimCharacter, CurrentHealth);
	DOREPLIFETIME(AXivSimCharacter, CurrentBarrier);
	DOREPLIFETIME(AXivSimCharacter, bCurrentStunning);
}

FString AXivSimCharacter::GetCharacterName() const
{
	if(AXivSimPlayerState* PS = Cast<AXivSimPlayerState>(GetPlayerState()))
	{
		return PS->GetPlayerName();
	}
	
	return Super::GetCharacterName();
}

void AXivSimCharacter::SetRolePlayType(EXivSimRolePlayType NewType)
{
	if(RolePlayType != NewType)
	{
		RolePlayType = NewType;
		OnRep_RolePlayType();
	}
}

void AXivSimCharacter::SetClassType(EXivSimClassType NewType)
{
	if(ClassType != NewType)
	{
		ClassType = NewType;
		OnRep_ClassType();
	}
}

void AXivSimCharacter::TryAddBuff(TSubclassOf<class UXivSimBuffBase> BuffClass)
{
	if (BuffClass)
	{
		ServerTryAddBuff(BuffClass);
	}
}

void AXivSimCharacter::SetXivSimInputMode(bool bStandardMode)
{
	ServerSetInputMode(bStandardMode);
	
	if (UXivSimGameUserSettings* GUS = UXivSimGameUserSettings::GetXivSimGameUserSettings())
	{
		GUS->SetUseStandardInputMode(bStandardMode);
	}
}

int32 AXivSimCharacter::GetHealth() const
{
	return CurrentHealth;
}

int32 AXivSimCharacter::GetMaxHealth() const
{
	return FMath::CeilToInt32((float)MaxHealth * GetAttributeModifier(EXivSimAttributeType::Attribute_Health));
}

void AXivSimCharacter::ServerTryAddBuff_Implementation(TSubclassOf<class UXivSimBuffBase> BuffClass)
{
	if (GetBuffComponent())
	{
		GetBuffComponent()->AddBuffByClass(BuffClass, this);
	}
}

void AXivSimCharacter::ServerSetInputMode_Implementation(bool bStandardMode)
{
	bStandardInputMode = bStandardMode;
	OnRep_StandardInputMode();
}

void AXivSimCharacter::SetHealth(int32 NewValue)
{
	if (HasAuthority())
	{
		int32 MinHealth = 0;
		if (AXivSimGameState* GameState = GetWorld()->GetGameState<AXivSimGameState>())
		{
			if (IsInTrainingMode())
			{
				MinHealth = 1;
			}
		}
		CurrentHealth = FMath::Clamp(NewValue, MinHealth, GetMaxHealth());
		OnPlayerHealthChange.Broadcast(CurrentHealth, GetMaxHealth());
	}
}

void AXivSimCharacter::SetGodMode(bool bNew)
{
	if(bIsInGodMode != bNew)
	{
		bIsInGodMode = true;
	}
}

bool AXivSimCharacter::IsInGodMode() const
{
	return bIsInGodMode;
}

void AXivSimCharacter::SetTrainingMode(bool bNew)
{
	if(bIsInTrainingMode != bNew)
	{
		bIsInTrainingMode = bNew;
	}
}

bool AXivSimCharacter::IsInTrainingMode() const
{
	return bIsInTrainingMode;
}

void AXivSimCharacter::SetMouseLeftHold(bool bHold)
{
	bMouseLeftHold = bHold;
}

void AXivSimCharacter::SetMouseRightHold(bool bHold)
{
	bMouseRightHold = bHold;

	if (bStandardInputMode)
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = bMouseRightHold;
	}
}

void AXivSimCharacter::AddAttributeModifier(EXivSimAttributeType Tag, float Value)
{
	if (!AttributeModifier.Contains(Tag))
	{
		AttributeModifier.Add(Tag, 1.f);
	}

	AttributeModifier[Tag] *= Value;

	if (Tag == EXivSimAttributeType::Attribute_Health)
	{
		SetHealth(CurrentHealth);
	}
}

float AXivSimCharacter::GetAttributeModifier(EXivSimAttributeType Tag) const
{
	if (const float* Value = AttributeModifier.Find(Tag))
	{
		return *Value;
	}

	return 1.f;
}

void AXivSimCharacter::AddDamageModifier(EXivSimDamageEffectType Tag, float Value)
{
	if (!DamageModifier.Contains(Tag))
	{
		DamageModifier.Add(Tag, 1.f);
	}

	DamageModifier[Tag] *= Value;
}

void AXivSimCharacter::ApplyDamageModifier(float& DamageAmount, const TSubclassOf<class UDamageType>& DamageTypeClass)
{
	if (DamageTypeClass)
	{
		UXivSimDamageType* DTPtr = Cast<UXivSimDamageType>(DamageTypeClass.GetDefaultObject());
		if (DTPtr && DTPtr->DamageEffectType != EXivSimDamageEffectType::Damage_All)
		{
			if (float* ModifierPtr = DamageModifier.Find(DTPtr->DamageEffectType))
			{
				DamageAmount *= (*ModifierPtr);
			}
		}
		if (float* ModifierPtr = DamageModifier.Find(EXivSimDamageEffectType::Damage_All))
		{
			DamageAmount *= (*ModifierPtr);
		}
	}

	if(GetRolePlayType() == EXivSimRolePlayType::RolePlay_Tank)
	{
		DamageAmount *= 0.8f; 
	}
	
	if (GetBuffComponent())
	{
		GetBuffComponent()->ProcessDamageModifier(DamageAmount, DamageTypeClass);
	}
}

void AXivSimCharacter::ApplyDamageBuff(AActor* DamageCauser, const TSubclassOf<class UDamageType>& DamageTypeClass)
{
	if (DamageTypeClass)
	{
		UXivSimDamageType* DTPtr = Cast<UXivSimDamageType>(DamageTypeClass.GetDefaultObject());
		if (DTPtr && DTPtr->DamageBuffClass)
		{
			GetBuffComponent()->AddBuffByClass(DTPtr->DamageBuffClass, DamageCauser);
		}
	}
}

FVector AXivSimCharacter::GetTargetRingRelativeLocation() const
{
	if(GetCapsuleComponent())
	{
		return FVector(0.f, 0.f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
	}
	return Super::GetTargetRingRelativeLocation();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AXivSimCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Mouse
		EnhancedInputComponent->BindAction(MouseLeftAction, ETriggerEvent::Triggered, this, &AXivSimCharacter::MouseLeftPressed);
		EnhancedInputComponent->BindAction(MouseLeftAction, ETriggerEvent::Completed, this, &AXivSimCharacter::MouseLeftReleased);
		EnhancedInputComponent->BindAction(MouseRightAction, ETriggerEvent::Triggered, this, &AXivSimCharacter::MouseRightPressed);
		EnhancedInputComponent->BindAction(MouseRightAction, ETriggerEvent::Completed, this, &AXivSimCharacter::MouseRightReleased);
		EnhancedInputComponent->BindAction(MouseMidAction, ETriggerEvent::Started, this, &AXivSimCharacter::MouseMidPressed);
		EnhancedInputComponent->BindAction(MouseMidAction, ETriggerEvent::Completed, this, &AXivSimCharacter::MouseMidReleased);
		EnhancedInputComponent->BindAction(MouseWheelAction, ETriggerEvent::Triggered, this, &AXivSimCharacter::Zoom);

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AXivSimCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AXivSimCharacter::Look);
		EnhancedInputComponent->BindAction(PadLookAction, ETriggerEvent::Triggered, this, &AXivSimCharacter::PadLook);
	}

}

void AXivSimCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = bStandardInputMode ? GetActorRotation() : GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

		if(FMath::Abs(MovementVector.Y) > 0.f)
		{
			bToggleAutoMoving = false;
		}
	}
}

void AXivSimCharacter::Look(const FInputActionValue& Value)
{
	if (IsMouseLeftHold() || IsMouseRightHold())
	{
		// input is a Vector2D
		FVector2D LookAxisVector = Value.Get<FVector2D>();

		if (Controller != nullptr)
		{
			// add yaw and pitch input to controller
			AddControllerYawInput(LookAxisVector.X);
			AddControllerPitchInput(LookAxisVector.Y);
		}
	}
}

void AXivSimCharacter::PadLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = !LookAxisVector.IsNearlyZero(0.01f);
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AXivSimCharacter::Zoom(const FInputActionValue& Value)
{
	float ZoomAxis = Value.Get<float>();
	if (GetCameraBoom())
	{
		float CurrentZoomValue = GetCameraBoom()->TargetArmLength;
		GetCameraBoom()->TargetArmLength = FMath::Clamp(CurrentZoomValue + ZoomAxis * ZoomSpeed, 0.f, MaxZoomDistance);
	}
}

bool AXivSimCharacter::CanJumpInternal_Implementation() const
{
	if(IsStunning())
	{
		return false;
	}
	return Super::CanJumpInternal_Implementation();
}

void AXivSimCharacter::MouseLeftPressed()
{
	SetMouseLeftHold(true);
	if(IsMouseRightHold())
	{
		if(!bHoldAutoMoving)
		{
			bHoldAutoMoving = true;
			bToggleAutoMoving = false;
		}
	}
}

void AXivSimCharacter::MouseLeftReleased()
{
	SetMouseLeftHold(false);
	bHoldAutoMoving = false;
}

void AXivSimCharacter::MouseRightPressed()
{
	SetMouseRightHold(true);
	if(IsMouseLeftHold())
	{
		if(!bHoldAutoMoving)
		{
			bHoldAutoMoving = true;
			bToggleAutoMoving = false;
		}
	}
}

void AXivSimCharacter::MouseRightReleased()
{
	SetMouseRightHold(false);
	bHoldAutoMoving = false;
}

void AXivSimCharacter::MouseMidPressed()
{
	bToggleAutoMoving = !bToggleAutoMoving;
}

void AXivSimCharacter::MouseMidReleased()
{
}

void AXivSimCharacter::TickAutoMoving(float DeltaSecond)
{
	if(IsLocallyControlled() && IsPlayerControlled())
	{
		if(bHoldAutoMoving || bToggleAutoMoving)
		{
			const FRotator Rotation = bStandardInputMode ? GetActorRotation() : GetController()->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(ForwardDirection, 1.f);
		}
	}
}

void AXivSimCharacter::SetPlayerMarker(EXivSimMarkerType MarkerType)
{
	if (AXivSimPlayerState* XivPS = Cast<AXivSimPlayerState>(GetPlayerState()))
	{
		XivPS->ServerSetCurrentMarkerType(MarkerType);
	}
}

void AXivSimCharacter::ClearAllPlayerMarker()
{
	if (AXivSimPlayerState* XivPS = Cast<AXivSimPlayerState>(GetPlayerState()))
	{
		XivPS->ServerClearAllPlayerMarker();
	}
}

