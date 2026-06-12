#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Class.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Engine/NetSerialization.h"
#include "BuffComponent.generated.h"

struct FBuffInfoContainer;
class UXivSimBuffBase;

USTRUCT()
struct FBuffInfo : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()
	
	FBuffInfo()
		:BuffPtr(nullptr)
	{

	}
	
	UPROPERTY()
	TObjectPtr<UXivSimBuffBase> BuffPtr;

	void PreReplicatedRemove(const struct FBuffInfoContainer& InArraySerializer);
	void PostReplicatedAdd(const struct FBuffInfoContainer& InArraySerializer);
	void PostReplicatedChange(const struct FBuffInfoContainer& InArraySerializer);
};

USTRUCT()
struct FBuffInfoContainer : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()
	
	FBuffInfoContainer()
		:OwnerBuffComponent(nullptr)
	{

	}
	
	UPROPERTY()
	TArray<FBuffInfo> BuffList;

	UPROPERTY(NotReplicated, SkipSerialization)
	TObjectPtr<UBuffComponent> OwnerBuffComponent;

	friend struct FBuffInfo;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FBuffInfo, FBuffInfoContainer>(BuffList, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits<FBuffInfoContainer> : public TStructOpsTypeTraitsBase2<FBuffInfoContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FAddBuffSignature, UBuffComponent, OnBuffAdd, UXivSimBuffBase*, InBuff);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FRemoveBuffSignature, UBuffComponent, OnBuffRemove, UXivSimBuffBase*, InBuff);

UCLASS(BlueprintType)
class UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBuffComponent();
	
	virtual void OnUnregister() override;

	const FBuffInfoContainer& GetBuffContainer() const { return BuffContainer; }
	UFUNCTION(BlueprintCallable)
	void AddBuff(class UXivSimBuffBase* Buff);
	UFUNCTION(BlueprintCallable)
	class UXivSimBuffBase* AddBuffByClass(TSubclassOf<class UXivSimBuffBase> BuffClass, UObject* BuffCreater);
	UFUNCTION(BlueprintPure)
	bool HasBuff(TSubclassOf<class UXivSimBuffBase> BuffClass);
	UFUNCTION(BlueprintCallable)
	void RemoveBuffByClass(TSubclassOf<class UXivSimBuffBase> BuffClass);
	UFUNCTION(BlueprintCallable)
	void RemoveBuff(class UXivSimBuffBase* Buff);
	UFUNCTION(BlueprintCallable)
	void ClearBuffs();
	UFUNCTION(BlueprintPure)
	class AXivSimCharacter* GetPlayerOwner() const;
	UFUNCTION(BlueprintPure)
	bool IsOwnerValid() const;
	void ProcessDamageModifier(float& DamageAmount, const TSubclassOf<class UDamageType>& DamageTypeClass);

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

public:
	UPROPERTY()
	FAddBuffSignature OnBuffAdd;

	UPROPERTY()
	FRemoveBuffSignature OnBuffRemove;

protected:
	UPROPERTY(Replicated, SkipSerialization)
	FBuffInfoContainer BuffContainer;
};