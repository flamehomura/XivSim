#pragma once

#include "GameFramework/PlayerState.h"
#include "XivSim.h"
#include "XivSimPlayerState.generated.h"

class AXivSimCharacterBase;

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FRolePlayTypeChangeSignature, AXivSimPlayerState, OnRolePlayTypeChange, EXivSimRolePlayType, RolePlayType);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FClassTypeChangeSignature, AXivSimPlayerState, OnClassTypeChange, EXivSimClassType, ClassType);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FMarkerTypeChangeSignature, AXivSimPlayerState, OnMarkerTypeChange, EXivSimMarkerType, MarkerType);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FPositionTypeChangeSignature, AXivSimPlayerState, OnPositionTypeChange, EXivSimPositionType, PosType);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FPlayerNameChangeSignature, AXivSimPlayerState, OnPlayerNameChange, FString, NewName);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FSelectedCharacterChangeSignature, AXivSimPlayerState, OnSelectedCharacterChange, AXivSimCharacterBase*, NewCharacter);

UCLASS()
class AXivSimPlayerState : public APlayerState
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual FString GetPlayerNameCustom() const override;
	void SetXivSimPlayerCustomName(const FString& NewName);

	EXivSimRolePlayType GetCurrentRolePlayType() const { return CurrentRolePlayType; }
	void SetCurrentRolePlayType(EXivSimRolePlayType NewType);

	EXivSimClassType GetCurrentClassType() const { return CurrentClassType; }
	UFUNCTION(BlueprintCallable)
	void SetCurrentClassType(EXivSimClassType NewType);

	UFUNCTION(BlueprintPure)
	EXivSimPositionType GetCurrentPositionType() const { return CurrentPositionType; }
	UFUNCTION(BlueprintCallable)
	void SetCurrentPositionType(EXivSimPositionType NewPos);
	
	EXivSimMarkerType GetCurrentMarkerType() const { return CurrentMarkerType; }
	void SetCurrentMarkerType(EXivSimMarkerType NewType);
	void SetCurrentSelectedCharacter(class AXivSimCharacterBase* NewCharacter);

	UFUNCTION(BlueprintCallable)
	void SetPlayerNameBP(const FString& NewName);

	UFUNCTION(Server, Reliable)
	void ServerSetPlayerName(const FString& NewName);
	
	UFUNCTION(Server, Reliable)
	void ServerSetPlayerClassType(EXivSimClassType NewType);

	UFUNCTION(Server, Reliable)
	void ServerSetCurrentMarkerType(EXivSimMarkerType NewType);
	
	UFUNCTION(Server, Reliable)
	void ServerSetPlayerPositionType(EXivSimPositionType NewType);

	UFUNCTION(Server, Reliable)
	void ServerClearAllPlayerMarker();
	
	UFUNCTION(Server, Reliable)
	void ServerSetCurrentSelectedCharacter(class AXivSimCharacterBase* NewSelectedCharacter);

protected:
	UFUNCTION()
	void OnRep_CurrentRolePlayType();
	UFUNCTION()
	void OnRep_CurrentClassType();
	UFUNCTION()
	void OnRep_CurrentMarkerType();
	UFUNCTION()
	void OnRep_CurrentPositionType();
	UFUNCTION()
	void OnRep_CurrentSelectedCharacter();
	UFUNCTION()
	virtual void OnRep_XivSimCustomPlayerName();

	class AXivSimPlayerController* GetMyPlayerController();

	UFUNCTION()
	void OnTargetCharacterCanBeSelectedStateChanged(class AXivSimCharacterBase* TargetChar, bool bNewState);

public:
	UPROPERTY()
	FRolePlayTypeChangeSignature OnRolePlayTypeChange;
	UPROPERTY()
	FClassTypeChangeSignature OnClassTypeChange;
	UPROPERTY()
	FMarkerTypeChangeSignature OnMarkerTypeChange;
	UPROPERTY()
	FPositionTypeChangeSignature OnPositionTypeChange;
	UPROPERTY()
	FPlayerNameChangeSignature OnPlayerNameChange;
	UPROPERTY()
	FSelectedCharacterChangeSignature OnSelectedCharacterChange;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentRolePlayType)
	EXivSimRolePlayType CurrentRolePlayType;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentClassType)
	EXivSimClassType CurrentClassType;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentMarkerType)
	EXivSimMarkerType CurrentMarkerType;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPositionType)
	EXivSimPositionType CurrentPositionType;

	UPROPERTY(ReplicatedUsing = OnRep_XivSimCustomPlayerName)
	FString XivSimCustomPlayerName;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentSelectedCharacter)
	TObjectPtr<AXivSimCharacterBase> CurrentSelectedCharacter;
};