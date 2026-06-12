#pragma once

#include "CoreMinimal.h"
#include "UObject/UnrealType.h"
#include "XivSim.h"
#include "GameFramework/GameStateBase.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Engine/NetSerialization.h"
#include "XivSimGameState.generated.h"

class AActor;
class AXivSimGameState;
class AXivSimCharacter;
class UXivSimDamageType;
class UXivSimPatternBase;

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FGameStateGodModeChangedSignature, AXivSimGameState, OnGodModeChanged, bool, bNewGod);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FGameStateTrainingModeChangedSignature, AXivSimGameState, OnTrainingModeChanged, bool, bNewTraining);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_TwoParams(FPatternPhaseChangedSignature, AXivSimGameState, OnCriticalPatternPhaseChanged, class UXivSimPatternBase*, Pattern, EPatternPhase, Phase);


class AXivSimPlayerState;

USTRUCT()
struct FPlayerTakeDamageInfo
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY()
	TObjectPtr<AXivSimCharacter> PlayerPtr;

	UPROPERTY()
	TObjectPtr<AActor> DamageCauser;

	UPROPERTY()
	TSubclassOf<UXivSimDamageType> DamageTypeClass;

	UPROPERTY()
	float DamageAmount;

	FPlayerTakeDamageInfo()
		: PlayerPtr(nullptr)
		, DamageCauser(nullptr)
		, DamageAmount(0.f)
	{}
};

struct FPatternDataContainer;

USTRUCT()
struct FPatternItem : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()
	
	FPatternItem()
		: Pattern(nullptr)
	{}

	UPROPERTY()
	TObjectPtr<UXivSimPatternBase> Pattern;

	void PreReplicatedRemove(const struct FPatternItemContainer& InArraySerializer);
	void PostReplicatedAdd(const struct FPatternItemContainer& InArraySerializer);
	void PostReplicatedChange(const struct FPatternItemContainer& InArraySerializer);
};

USTRUCT()
struct FPatternItemContainer : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()
	
	FPatternItemContainer()
		: OwnerGameState(nullptr)
	{}
	
	void Clear();

	UPROPERTY()
	TArray<FPatternItem> PatternList;

	UPROPERTY(NotReplicated)
	TObjectPtr<AXivSimGameState> OwnerGameState;

	friend struct FPatternItem;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FPatternItem, FPatternItemContainer>(PatternList, DeltaParms, *this);
	}
};


template<>
struct TStructOpsTypeTraits<FPatternItemContainer> : public TStructOpsTypeTraitsBase2<FPatternItemContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

UCLASS(minimalapi)
class AXivSimGameState : public AGameStateBase
{
	GENERATED_UCLASS_BODY()

public:
	virtual void HandleBeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

	void NotifyPlayerTakeDamage(class AXivSimCharacter* PlayerPtr, float DamageCount, const TSubclassOf<class UXivSimDamageType> DamageTypeClass, AActor* DamageCauser);
	
	UFUNCTION(NetMulticast, Unreliable)
	void BroadcastPlayerDamageInfo(FPlayerTakeDamageInfo DamageInfo);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	void AddPattern(class UXivSimPatternBase* InPattern);
	void RemovePattern(class UXivSimPatternBase* InPattern);

	UFUNCTION(BlueprintCallable)
	void SetIgnorePlayerDamage(bool NewValue);
	UFUNCTION(BlueprintPure)
	bool IsIgnorePlayerDamage() const { return bIgnorePlayerDamage; }
	UFUNCTION(BlueprintCallable)
	void SetTrainingMode(bool NewValue);
	UFUNCTION(BlueprintPure)
	bool IsTrainingMode() const { return bIsTrainingMode; }

protected:
	void UpdateServerWindowTitle();
	UFUNCTION()
	void OnRep_IgnorePlayerDamage();
	UFUNCTION()
	void OnRep_TrainingMode();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnSetIgnorePlayerDamage(bool bNew);
	UFUNCTION(BlueprintImplementableEvent)
	void OnSetTrainingMode(bool bNew);

public:
	UPROPERTY(BlueprintAssignable)
	FGameStateGodModeChangedSignature OnGodModeChanged;
	UPROPERTY(BlueprintAssignable)
	FGameStateTrainingModeChangedSignature OnTrainingModeChanged;
	UPROPERTY(BlueprintAssignable)
	FPatternPhaseChangedSignature OnCriticalPatternPhaseChanged;

protected:
	UPROPERTY()
	TArray<TObjectPtr<AXivSimCharacter>> PlayerList;
	TMap<EXivSimRolePlayType, TArray<TWeakObjectPtr<AXivSimCharacter>>> PlayerRolePlayTypeMap;

	UPROPERTY(Replicated, SkipSerialization)
	FPatternItemContainer PatternItemContainer;

	UPROPERTY(ReplicatedUsing=OnRep_IgnorePlayerDamage)
	bool bIgnorePlayerDamage = false;
	UPROPERTY(ReplicatedUsing=OnRep_TrainingMode)
	bool bIsTrainingMode = false;
};
