#pragma once

#include "CoreMinimal.h"
#include "XivSim.h"
#include "XivSimPatternBase.generated.h"

class AActor;
class AXivSimCharacter;
class UDecalComponent;

UENUM(BlueprintType)
enum class EPatternPhase : uint8
{
	Phase_None,
	Phase_Preparing,
	Phase_ShowRange,
	Phase_TargetLock,
	Phase_Effect,
	Phase_Field,
};

UCLASS(BlueprintType, Blueprintable)
class UXivSimPatternBase : public UObject
{
	GENERATED_BODY()

public:
	UXivSimPatternBase();

	virtual void OnActivated();
	virtual void OnDeactivated();

	virtual bool IsSupportedForNetworking() const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SetPatternID(int32 InID);
	int32 GetPatternID() const;
	virtual void SetPatternTransform(FTransform Trans);
	virtual void SetPatternOwner(AActor* InOwner);
	virtual void SetPatternTarget(AActor* InTarget);
	virtual void SetExcludePlayerList(const TArray<class AXivSimCharacter*>& InPlayerList);
	const FXivSimPatternData* GetPatternData() const { return DataInfo; }
	UFUNCTION(BlueprintPure)
	const FXivSimPatternData& GetPatternDataRef() const { return *DataInfo; }

	UFUNCTION()
	void BeginPreparingPhase();
	UFUNCTION(BlueprintImplementableEvent)
	void BeginPreparingPhaseBP();

	UFUNCTION()
	void BeginShowRangePhase();
	UFUNCTION(BlueprintImplementableEvent)
	void BeginShowRangePhaseBP();

	UFUNCTION()
	void BeginLockTargetPhase();
	UFUNCTION(BlueprintImplementableEvent)
	void BeginLockTargetPhaseBP();

	UFUNCTION()
	void BeginEffectPhase();
	UFUNCTION(BlueprintImplementableEvent)
	void BeginEffectPhaseBP();

	UFUNCTION()
	void BeginFieldPhase();
	UFUNCTION(BlueprintImplementableEvent)
	void BeginFieldPhaseBP();

	UFUNCTION()
	void BeginDestroyPhase();

protected:
	UFUNCTION(BlueprintPure)
	class AXivSimGameState* GetOwnerGameState();
	UFUNCTION(BlueprintPure)
	bool HasAuthority();

	void SetTimerToNextPhase();
	UFUNCTION()
	void FieldEffectTick(bool bFirstTick = false);
	UFUNCTION()
	void FieldEffectTickEnd();
	
	void CreateRangeDecal(class UMaterialInterface* DecalMaterial, float DecalLifeSpan, bool bFieldDecal = false);
	void ModifyDecalRange(bool bFieldDecal = false);
	class UMaterialInstanceDynamic* GetRangeDecalDynamicMaterial();

	UFUNCTION()
	void OnRep_PatternID();

	UFUNCTION()
	void OnRep_AffectTransform();

	UFUNCTION()
	void OnRep_CurrentPatternPhase();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_PatternID)
	int32 PatternID;

	UPROPERTY(Replicated, BlueprintReadOnly)
	TObjectPtr<AActor> PatternOwner;

	UPROPERTY(Replicated, BlueprintReadOnly)
	TObjectPtr<AActor> PatternTarget;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentPatternPhase)
	EPatternPhase CurrentPatternPhase;

	FXivSimPatternData* DataInfo;

	FTransform WorldTransform;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AffectTransform)
	FTransform AffectTransform;

	UPROPERTY()
	TArray<TObjectPtr<AXivSimCharacter>> ExcludePlayers;

	UPROPERTY(Replicated)
	bool bCriticalEffect;

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<AXivSimCharacter>> AffectPlayerArray;

	FTimerHandle PhaseChangeTimerHandle;
	FTimerHandle FieldTickTimerHandle;
	FTimerHandle FieldTickEndTimerHandle;

	UPROPERTY()
	TObjectPtr<UDecalComponent> RangeDecal;

	float BaseDecalSize;
	float BaseDecalDepth;

	float FieldEffectTickInterval = 0.1f;
	float FieldEffectTickStartTime = 0.f;
	float FieldEffectTickToleranceTime = 1.f;
	UPROPERTY()
	TArray<TObjectPtr<AXivSimCharacter>> FieldRangeTolerancePlayerArray;
	UPROPERTY()
	TArray<TObjectPtr<AXivSimCharacter>> FieldRangePlayerArray;
};