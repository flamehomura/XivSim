#pragma once

#include "XivSimManagerBase.h"
#include "XivSim.h"
#include "UObject/UnrealType.h"
#include "XivSimPlayerManager.generated.h"

class AXivSimPlayerState;
class UXivSimDamageType;
class AXivSimCharacter;

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FAddPlayerStateSignature, UXivSimPlayerManager, OnPlayerStateAdd, AXivSimPlayerState*, InPlayerState);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FRemovePlayerStateSignature, UXivSimPlayerManager, OnPlayerStateRemove, AXivSimPlayerState*, InPlayerState);

UCLASS()
class UXivSimPlayerManager : public UXivSimManagerBase
{
	GENERATED_BODY()
	
public:
	UXivSimPlayerManager();
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetPlayerManager", DefaultToSelf = "WorldContextObject"))
	static UXivSimPlayerManager* Get(const UObject* WorldContextObject);

	virtual void Init() override;
	virtual void UnInit() override;

	EXivSimRolePlayType GetClassRolePlayType(EXivSimClassType InType) const;

	TArray<AXivSimPlayerState*> GetPlayerStateArray() const { return PlayerStateArray; }
	void AddPlayerState(AXivSimPlayerState* InPlayerState);
	void RemovePlayerState(AXivSimPlayerState* InPlayerState);
	void AddPlayer(AXivSimCharacter* InPlayer);
	void RemovePlayer(AXivSimCharacter* InPlayer);
	bool CheckPlayerInArena(AXivSimCharacter* InPlayer);
	TSubclassOf<UXivSimDamageType> GetOutOfArenaDamageType() const;

	void SetPlayerMarkerType(AXivSimPlayerState* InPlayerState, EXivSimMarkerType MarkerType);
	void ClearAllPlayerMarker();

	UFUNCTION(BlueprintCallable)
	TArray<AXivSimCharacter*> GetAllPlayer() const { return PlayerList; }
	UFUNCTION(BlueprintCallable)
	TArray<AXivSimCharacter*> GetRandomPlayer(int32 Count, EXivSimRolePlayType RoleType, bool bAllowDuplicate = false) const; 
	TArray<AXivSimCharacter*> GetRandomPlayerFromList(int32 Count, TArray<AXivSimCharacter*>& InPlayerArray, bool bAllowDuplicate = false) const;
	UFUNCTION(BlueprintCallable)
	TArray<AXivSimCharacter*> GetRandomPlayerFromList(int32 Count, const TArray<class AXivSimCharacter*>& InPlayerArray, TArray<AXivSimCharacter*>& OutPlayerArray, bool bAllowDuplicate = false);
	UFUNCTION(BlueprintCallable)
	TArray<AXivSimCharacter*> GetRandomPlayerInRange(int32 Count, int32 MinCount, FTransform CenterTrans, EXivSimPatternRangeType RangeType, float RangeParam1, float RangeParam2, 
		const TArray<AXivSimCharacter*>& ExcludePlayers, bool bAllowDuplicate = false, bool bAllowOutRange = false) const;
	UFUNCTION(BlueprintCallable)
	TArray<AXivSimCharacter*> GetFarestPlayerFrom(int32 Count, FVector FromLocation) const;
	UFUNCTION(BlueprintCallable)
	TArray<AXivSimCharacter*> GetNearestPlayerFrom(int32 Count, FVector FromLocation) const;
	
	UFUNCTION(BlueprintCallable)
	TArray<AXivSimCharacter*> GetFarestPlayerInListFrom(int32 Count, FVector FromLocation, const TArray<AXivSimCharacter*>& InPlayerArray, TArray<AXivSimCharacter*>& OutPlayerArray);
	UFUNCTION(BlueprintCallable)
	TArray<AXivSimCharacter*> GetNearestPlayerInListFrom(int32 Count, FVector FromLocation, const TArray<AXivSimCharacter*>& InPlayerArray, TArray<AXivSimCharacter*>& OutPlayerArray);

	UFUNCTION(BlueprintCallable)
	TArray<AXivSimCharacter*> GetFarestPlayerFromLine(int32 Count, FVector LineLocation, FVector LineDir) const;
	UFUNCTION(BlueprintCallable)
	TArray<AXivSimCharacter*> GetNearestPlayerFromLine(int32 Count, FVector LineLocation, FVector LineDir) const;
	UFUNCTION(BlueprintCallable)
	TArray<AXivSimCharacter*> GetFarestPlayerInListFromLine(int32 Count, FVector LineLocation, FVector LineDir, const TArray<AXivSimCharacter*>& InPlayerArray, TArray<class AXivSimCharacter*>& OutPlayerArray);
	UFUNCTION(BlueprintCallable)
	TArray<AXivSimCharacter*> GetNearestPlayerInListFromLine(int32 Count, FVector LineLocation, FVector LineDir, const TArray<AXivSimCharacter*>& InPlayerArray, TArray<class AXivSimCharacter*>& OutPlayerArray);

protected:
	UFUNCTION()
	void OnPlayerDead(class AXivSimCharacter* PlayerPtr);
	UFUNCTION()
	void OnPlayerPawnSet(class APlayerState* PlayerState, class APawn* NewPawn, class APawn* OldPawn);

	TArray<AXivSimCharacter*> GetPlayerInCircle(FVector CenterLocation, float Radius) const;
	TArray<AXivSimCharacter*> GetPlayerInSector(FTransform CenterTrans, float Radius, float Degree) const;
	TArray<AXivSimCharacter*> GetPlayerInRing(FVector CenterLocation, float Radius, float InnerRadius) const;
	TArray<AXivSimCharacter*> GetPlayerInLine(FTransform CenterTrans, float Length, float Width) const;
	TArray<AXivSimCharacter*> GetPlayerInHalfLine(FTransform CenterTrans, float Length, float Width) const;
	TArray<AXivSimCharacter*> GetPlayerInCross(FTransform CenterTrans, float Length, float Width) const;
	TArray<AXivSimCharacter*> GetPlayerInWing(FTransform CenterTrans, float Length, float Width) const;
	TArray<AXivSimCharacter*> GetPlayerInEvilEye(FVector CenterLocation, float Radius, float Degree, bool bAnti = false) const;

	UFUNCTION()
	void HealAllPlayers();

public:
	UPROPERTY()
	FAddPlayerStateSignature OnPlayerStateAdd;

	UPROPERTY()
	FRemovePlayerStateSignature OnPlayerStateRemove;

protected:
	UPROPERTY()
	TArray<TObjectPtr<AXivSimPlayerState>> PlayerStateArray;

	UPROPERTY()
	TMap<EXivSimMarkerType, TObjectPtr<AXivSimPlayerState>> PlayerMarkerMap;

	UPROPERTY()
	TArray<TObjectPtr<AXivSimCharacter>> PlayerList;
	TMap<EXivSimRolePlayType, TArray<TWeakObjectPtr<AXivSimCharacter>>> PlayerRolePlayTypeMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<EXivSimClassType, EXivSimRolePlayType> ClassTypeToRolePlayTypeMap;

	UPROPERTY(EditAnywhere)
	FVector ArenaCenterPose;

	UPROPERTY(EditAnywhere)
	float ArenaRadius;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UXivSimDamageType> OutOfArenaDamageType;

	UPROPERTY(EditAnywhere)
	float PlayerHealingInterval;

	UPROPERTY(EditAnywhere)
	float PlayerHealingAmout;

	FTimerHandle PlayerHealingTimerHandle;
};