#pragma once

#include "CoreMinimal.h"
#include "XivSim.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Class.h"
#include "Components/ActorComponent.h"
#include "XivSimActionComponent.generated.h"

class AXivSimCharacterBase;

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FActionStartCastingSignature, UXivSimActionComponent, OnActionStartCasting, const FXivSimActionData&, ActionData);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FActionCancelCastingSignature, UXivSimActionComponent, OnActionCancelCasting, const FXivSimActionData&, ActionData);

USTRUCT()
struct FRepActionInfo
{
	GENERATED_BODY()

	UPROPERTY()
	int32 ActionID;
	UPROPERTY()
	uint32 ActionProcessID;
	UPROPERTY()
	TObjectPtr<AXivSimCharacterBase> ActionTargetChar;
	UPROPERTY()
	bool bCancelled;

	FRepActionInfo()
		: ActionID(0)
		, ActionProcessID(0)
		, ActionTargetChar(nullptr)
		, bCancelled(false)
	{}
};

USTRUCT()
struct FActionCastingInfo
{
	GENERATED_BODY()

	UPROPERTY()
	int32 ActionID;
	UPROPERTY()
	float CastingTime;
	UPROPERTY()
	FTransform CastingTransform;
	UPROPERTY()
	TObjectPtr<AXivSimCharacterBase> CastingTarget;

	bool IsCasting(float ToleranceTime = 0.f) const
	{
		return ActionID > 0 && CastingTime > ToleranceTime;
	}

	void Clear()
	{
		ActionID = 0;
		CastingTime = 0.f;
		CastingTransform = FTransform::Identity;
		CastingTarget = nullptr;
	}
	
	FActionCastingInfo()
		: ActionID(0)
		, CastingTime(0.f)
		, CastingTarget(nullptr)
	{}
};

USTRUCT()
struct FCachedActionInput
{
	GENERATED_BODY()
	
	UPROPERTY()
	int32 ActionID;
	UPROPERTY()
	TObjectPtr<AXivSimCharacterBase> ActionTarget;

	bool IsValid()
	{
		return ActionID > 0;
	}

	void Clear()
	{
		ActionID = 0;
		ActionTarget = nullptr;
	}

	FCachedActionInput()
		: ActionID(0)
		, ActionTarget(nullptr)
	{}
};

UCLASS(BlueprintType)
class UXivSimActionComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UXivSimActionComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void TryActivateAction(const int32& InActionID, class AXivSimCharacterBase* TargetChar);
	UFUNCTION(Server, Reliable)
	void ServerActivateAction(const int32& InActionID, class AXivSimCharacterBase* TargetChar);
	
	bool CheckActionInCD(const FXivSimActionData& InActionData, float ToleranceTime = 0.f);
	bool CheckActionTarget(const FXivSimActionData& InActionData, class AXivSimCharacterBase*& InTarget);
	float GetActionCDPercent(const FXivSimActionData& InActionData);
	
	void TryCacheActionInput(const int32& InActionID, class AXivSimCharacterBase* TargetChar);

	bool IsCasting(float ToleranceTime = 0.f) const;

protected:
	void OnRegister() override;
	void OnUnregister() override;
	
	class AXivSimCharacter* GetOwnerPlayer();
	
	void TransformUpdated(USceneComponent* InRootComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport);
	
	UFUNCTION()
	void OnRep_CurrentActionInfo();

	void ActivateGlobalCoolingDown(float CDTime);
	void ActivateActionCoolingDown(const int32& ActionID, float CDTime);
	void ActivateActionCasting(const int32& ActionID, float CastingTime, class AXivSimCharacterBase* InTarget);

	const FXivSimActionData* GetActionData(const int32& InActionID) const;
	void UpdateActionCoolingDown(const float& DeltaTime);
	void UpdateActionCasting(const float& DeltaTime);

	void OnGlobalCoolingDownFinished();
	void OnActionCoolingDownFinished(const int32& ActionID);

	bool CheckInGlobalCD(float ToleranceTime = 0.f) const;
	void ExecuteAction(const FXivSimActionData& InActionData, class AXivSimCharacterBase* InTarget);

	float GetCDCheckThreshold() const;
	float GetRangeCheckThreshold() const;

	bool CastingCanBeInterrupted() const;
	void CheckCastingInterrupted();
	void InterruptCurrentCasting();
	void EndCurrentCasting();
	
	UFUNCTION()
	void OnCastingTargetCanBeSelectedStateChanged(class AXivSimCharacterBase* TargetChar, bool bNewState);

public:
	UPROPERTY()
	FActionStartCastingSignature OnActionStartCasting;
	UPROPERTY()
	FActionCancelCastingSignature OnActionCancelCasting;

protected:
	UPROPERTY(ReplicatedUsing=OnRep_CurrentActionInfo)
	FRepActionInfo CurrentActionInfo;
	
	UPROPERTY(EditDefaultsOnly)
	float DSActionCDCheckThreshold = 0.1f;
	UPROPERTY(EditDefaultsOnly)
	float DSActionRangeCheckThreshold = 10.f;
	
	UPROPERTY(EditDefaultsOnly)
	float ActionInputCacheToleranceTime = 0.2f;

	float GlobalCoolingDownTime;
	TMap<int32, float> ActionCoolingDownTimeMap;
	TArray<int32> ActionCoolingDownTimeOutArray;
	
	UPROPERTY()
	FActionCastingInfo ActionCastingTime;
	UPROPERTY()
	FCachedActionInput CachedActionInput;
};
