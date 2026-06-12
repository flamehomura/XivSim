// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "XivSimCharacterBase.h"
#include "InputActionValue.h"
#include "XivSim.h"
#include "XivSimCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
class UBuffComponent;
class UXivSimActionComponent;
class UInputAction;

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_TwoParams(FHealthChangeSignature, AXivSimCharacter, OnPlayerHealthChange, int32, CurrentHealth, int32, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FOnDeadSignature, AXivSimCharacter, OnPlayerDead, AXivSimCharacter*, PlayerPtr);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE(FOnUndyingSignature, AXivSimCharacter, OnPlayerUndying);

UCLASS(config=Game)
class AXivSimCharacter : public AXivSimCharacterBase
{
	GENERATED_UCLASS_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> MarkerWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), SkipSerialization)
	TObjectPtr<UBuffComponent> BuffComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), SkipSerialization)
	TObjectPtr<UXivSimActionComponent> ActionComponent;

#pragma region Input

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float ZoomSpeed = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float MaxZoomDistance = 2500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MouseLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MouseRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MouseWheelAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MouseMidAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> PadLookAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

#pragma endregion

public:
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual bool ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;
	void SetCurrentCharacterState(EXivSimCharacterState NewState);
	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;
	virtual bool CheckStillInWorld() override;
	UFUNCTION(BlueprintCallable)
	void SetStun(bool bNewStun);
	bool IsStunning() const;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	void PadLook(const FInputActionValue& Value);
	void Zoom(const FInputActionValue& Value);
	virtual bool CanJumpInternal_Implementation() const override;

	void MouseLeftPressed();
	void MouseLeftReleased();
	void MouseRightPressed();
	void MouseRightReleased();
	void MouseMidPressed();
	void MouseMidReleased();

	void TickAutoMoving(float DeltaSecond);

	UFUNCTION(BlueprintCallable)
	void SetPlayerMarker(EXivSimMarkerType MarkerType);
	UFUNCTION(BlueprintCallable)
	void ClearAllPlayerMarker();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState) override;
	void NotifyCurrentCharacterStateChanged();

	UFUNCTION()
	void OnRep_RolePlayType();
	
	UFUNCTION()
	void OnRep_ClassType();
	
	UFUNCTION()
	void OnRep_CurrentState();

	UFUNCTION()
	void OnRep_CurrentHealth();

	UFUNCTION()
	void OnRep_StandardInputMode();

	virtual void OnRep_PlayerState() override;

	void OnDead();

	UFUNCTION(BlueprintImplementableEvent)
	void NotifyRolePlayTypeChanged();
	
	UFUNCTION(BlueprintImplementableEvent)
	void NotifyClassTypeChanged();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual FString GetCharacterName() const override;
	
	UFUNCTION(BlueprintPure)
	EXivSimRolePlayType GetRolePlayType() const { return RolePlayType; }
	void SetRolePlayType(EXivSimRolePlayType NewType);

	UFUNCTION(BlueprintPure)
	EXivSimClassType GetClassType() const { return ClassType; }
	void SetClassType(EXivSimClassType NewType);

	UFUNCTION(BlueprintPure)
	UBuffComponent* GetBuffComponent() { return BuffComponent; }
	
	UFUNCTION(BlueprintPure)
	UXivSimActionComponent* GetActionComponent() { return ActionComponent; }

	UFUNCTION(BlueprintCallable)
	void TryAddBuff(TSubclassOf<class UXivSimBuffBase> BuffClass);

	UFUNCTION(Server, Reliable)
	void ServerTryAddBuff(TSubclassOf<class UXivSimBuffBase> BuffClass);

	UFUNCTION(BlueprintCallable)
	void SetXivSimInputMode(bool bStandardMode);

	UFUNCTION(Server, Reliable)
	void ServerSetInputMode(bool bStandardMode);
	bool IsStandardInputMode() const { return bStandardInputMode; }

	UFUNCTION(BlueprintPure)
	int32 GetHealth() const;

	UFUNCTION(BlueprintPure)
	int32 GetMaxHealth() const;
	void SetHealth(int32 NewValue);
	
	UFUNCTION(BlueprintCallable)
	void SetGodMode(bool bNew);
	UFUNCTION(BlueprintPure)
	bool IsInGodMode() const;
	UFUNCTION(BlueprintCallable)
	void SetTrainingMode(bool bNew);
	UFUNCTION(BlueprintPure)
	bool IsInTrainingMode() const;

	bool IsMouseLeftHold() const { return bMouseLeftHold; }
	bool IsMouseRightHold() const { return bMouseRightHold; }
	void SetMouseLeftHold(bool bHold);
	void SetMouseRightHold(bool bHold);

	void AddAttributeModifier(EXivSimAttributeType Tag, float Value);
	float GetAttributeModifier(EXivSimAttributeType Tag) const;

	void AddDamageModifier(EXivSimDamageEffectType Tag, float Value);
	void ApplyDamageModifier(float& DamageAmount, const TSubclassOf<class UDamageType>& DamageTypeClass);
	void ApplyDamageBuff(AActor* DamageCauser, const TSubclassOf<class UDamageType>& DamageTypeClass);

	virtual FVector GetTargetRingRelativeLocation() const override;
	
public:
	UPROPERTY()
	FHealthChangeSignature OnPlayerHealthChange;
	UPROPERTY()
	FOnDeadSignature OnPlayerDead;
	UPROPERTY(BlueprintAssignable)
	FOnUndyingSignature OnPlayerUndying;

protected:
	bool bIsInGodMode;
	bool bIsInTrainingMode;
	
	bool bMouseLeftHold = false;
	bool bMouseRightHold = false;
	bool bHoldAutoMoving = false;
	bool bToggleAutoMoving = false;

	UPROPERTY(ReplicatedUsing = OnRep_StandardInputMode, EditDefaultsOnly)
	bool bStandardInputMode = true;

	UPROPERTY(ReplicatedUsing = OnRep_RolePlayType)
	EXivSimRolePlayType RolePlayType;
	
	UPROPERTY(ReplicatedUsing = OnRep_ClassType)
	EXivSimClassType ClassType;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentState)
	EXivSimCharacterState CurrentState;

	UPROPERTY(Replicated, EditDefaultsOnly)
	int32 MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	int32 CurrentHealth;

	UPROPERTY(Replicated)
	uint32 CurrentBarrier;
	
	UPROPERTY(Replicated)
	bool bCurrentStunning;

	UPROPERTY()
	TMap<EXivSimAttributeType, float> AttributeModifier;

	UPROPERTY()
	TMap<EXivSimDamageEffectType, float> DamageModifier;
};
