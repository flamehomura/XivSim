// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Engine/DataTable.h"
#include "XivSim.generated.h"

#define XIVSIM_MAX_PLAYER_COUNT 8

class UMaterialInterface;
class UNiagaraSystem;
class UAnimMontage;
class UTexture2D;

UENUM(BlueprintType)
enum class EXivSimRolePlayType : uint8
{
	RolePlay_Tank,
	RolePlay_Healer,
	RolePlay_DPS,
	RolePlay_TankOrHealer,
	RolePlay_All,
};

UENUM(BlueprintType)
enum class EXivSimClassType : uint8
{
	Class_None,
	Class_Warrior,
	Class_Astrologian,
	Class_Viper,
	Class_Dancer,
	Class_Pictomancer,
};

UENUM(BlueprintType)
enum class EXivSimPositionType : uint8
{
	MT,
	ST,
	H1,
	H2,
	D1,
	D2,
	D3,
	D4,
};

UENUM(BlueprintType)
enum class EXivSimDirectionType : uint8
{
	Direction_Forward,
	Direction_Backward,
	Direction_Left,
	Direction_Right,
};

UENUM(BlueprintType)
enum class EXivSimAttributeType : uint8
{
	Attribute_Health,
	Attribute_MoveSpeed,
};

UENUM(BlueprintType)
enum class EXivSimDamageEffectType : uint8
{
	Damage_All,
	Damage_Physical,
	Damage_Magical,
	Damage_Fire,
	Damage_Thunder,
	Damage_Wind,
	Damage_Special,
	Damage_Alive,
	Damage_Death,
};

UENUM(BlueprintType)
enum class EXivSimPatternType : uint8
{
	PatternType_AOE,
	PatternType_Stack,
	PatternType_Field,
};

UENUM(BlueprintType)
enum class EXivSimPatternRangeType : uint8
{
	RangeType_All,
	RangeType_Circle,
	RangeType_Sector,
	RangeType_Line,
	RangeType_HalfLine,
	RangeType_Ring,
	RangeType_Cross,
	RangeType_Wing,
	RangeType_EvilEye,
	RangeType_EvilEyeAnti,
	RangeType_Attach,
};

UENUM(BlueprintType)
enum class EXivSimPatternPriorityType : uint8
{
	PatternType_All,
	PatternType_Nearest,
	PatternType_Farest,
	PatternType_LineNearest,
	PatternType_LineFarest,
};

UENUM(BlueprintType)
enum class EXivSimMarkerType : uint8
{
	Marker_None,
	Marker_Attack,
	Marker_Attack1,
	Marker_Attack2,
	Marker_Attack3,
	Marker_Attack4,
	Marker_Attack5,
	Marker_Attack6,
	Marker_Attack7,
	Marker_Attack8,
	Marker_Chain,
	Marker_Chain1,
	Marker_Chain2,
	Marker_Chain3,
	Marker_Chain4,
	Marker_Chain5,
	Marker_Chain6,
	Marker_Chain7,
	Marker_Chain8,
	Marker_Ignore,
	Marker_Ignore1,
	Marker_Ignore2,
	Marker_Ignore3,
	Marker_Ignore4,
	Marker_Ignore5,
	Marker_Ignore6,
	Marker_Ignore7,
	Marker_Ignore8,
	Marker_Circle,
	Marker_Square,
	Marker_Plus,
	Marker_Triangle
};

UENUM(BlueprintType)
enum class EXivSimCharacterState : uint8
{
	State_Alive,
	State_Dead,
};

class UXivSimPatternBase;
class UXivSimBuffBase;
class UXivSimDamageType;

USTRUCT(BlueprintType)
struct FXivSimPatternData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 PatternID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText PatternName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCriticalPattern;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EXivSimPatternType PatternType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UXivSimPatternBase> PatternClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EXivSimPatternRangeType RangeType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RangeParam1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RangeParam2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EXivSimPatternRangeType FieldRangeType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FieldRangeParam1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FieldRangeParam2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> RangeDecalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> EffectDecalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> FieldDecalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 AffectTargetCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MinAffectTargetCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxAffectTargetCount;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EXivSimPatternPriorityType AffectTargetPriority;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bExcludeOwnerPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bExcludeTargetPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAllowDuplicatedTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAllowOutOfRangeTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PatternRangeDelay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PatternDamageDelay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PatternEffectDelay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PatternFieldDelay;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PatternDestroyDelay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PatternEffectLifeSpan;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PatternFieldLifeSpan;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UXivSimBuffBase>> BuffClasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UXivSimDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 DamageAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> DamageEffectTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCriticalAffectAll;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UXivSimBuffBase>> CriticalBuffClasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UXivSimDamageType> CriticalDamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 CriticalDamageAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> CriticalDamageEffectTemplate;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UXivSimBuffBase>> FieldBuffClasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UXivSimDamageType> FieldDamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 FieldDamageAmount;


	FXivSimPatternData()
		: PatternID(0)
		, bCriticalPattern(false)
		, PatternType(EXivSimPatternType::PatternType_AOE)
		, RangeType(EXivSimPatternRangeType::RangeType_Circle)
		, RangeParam1(0.f)
		, RangeParam2(0.f)
		, FieldRangeType(EXivSimPatternRangeType::RangeType_All)
		, FieldRangeParam1(0.f)
		, FieldRangeParam2(0.f)
		, RangeDecalMaterial(nullptr)
		, EffectDecalMaterial(nullptr)
		, FieldDecalMaterial(nullptr)
		, AffectTargetCount(XIVSIM_MAX_PLAYER_COUNT)
		, MinAffectTargetCount(0)
		, MaxAffectTargetCount(XIVSIM_MAX_PLAYER_COUNT)
		, AffectTargetPriority(EXivSimPatternPriorityType::PatternType_All)
		, bExcludeOwnerPlayer(false)
		, bExcludeTargetPlayer(false)
		, bAllowDuplicatedTarget(false)
		, bAllowOutOfRangeTarget(false)
		, PatternRangeDelay(0.f)
		, PatternDamageDelay(0.f)
		, PatternEffectDelay(0.f)
		, PatternFieldDelay(0.f)
		, PatternDestroyDelay(3.f)
		, PatternEffectLifeSpan(0.f)
		, PatternFieldLifeSpan(0.f)
		, DamageAmount(0)
		, DamageEffectTemplate(nullptr)
		, bCriticalAffectAll(true)
		, CriticalDamageAmount(0)
		, CriticalDamageEffectTemplate(nullptr)
		, FieldDamageAmount(0)
	{}
};


USTRUCT(BlueprintType)
struct FXivSimActionData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	int32 ActionID;

	UPROPERTY(EditAnywhere)
	FText ActionName;
	
	UPROPERTY(EditAnywhere)
	uint32 ClassTypeBitMask;
	
	UPROPERTY(EditAnywhere)
	bool bWeaponAction;
	
	UPROPERTY(EditAnywhere)
	bool bCastingAction;
	
	UPROPERTY(EditAnywhere)
	bool bNeedTarget;
	
	UPROPERTY(EditAnywhere)
	bool bAffectSelf;
	
	UPROPERTY(EditAnywhere)
	bool bAffectHostile;
	
	UPROPERTY(EditAnywhere)
	bool bAffectFriendly;
	
	UPROPERTY(EditAnywhere)
	float CastingTime;
	
	UPROPERTY(EditAnywhere)
	float CastingInterruptibleToleranceTime;
	
	UPROPERTY(EditAnywhere)
	float RecastTime;
	
	UPROPERTY(EditAnywhere)
	float ActionRange;
	
	UPROPERTY(EditAnywhere)
	float EffectRange;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> ActionAnimMontage;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> ActionIcon;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UXivSimActionBase> ActionClass;

	bool IsValid() const
	{
		return ActionID > 0;
	}
	
	FXivSimActionData()
		: ActionID(0)
		, ClassTypeBitMask(0)
		, bWeaponAction(true)
		, bCastingAction(false)
		, bNeedTarget(true)
		, bAffectSelf(false)
		, bAffectHostile(true)
		, bAffectFriendly(false)
		, CastingTime(0.f)
		, CastingInterruptibleToleranceTime(0.5f)
		, RecastTime(0.f)
		, ActionRange(300.f)
		, EffectRange(0.f)
		, ActionAnimMontage(nullptr)
		, ActionIcon(nullptr)
	{
	}
};