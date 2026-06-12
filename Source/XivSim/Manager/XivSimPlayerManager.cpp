#include "XivSimPlayerManager.h"
#include "XivSimWorldSettings.h"
#include "XivSimCharacter.h"
#include "XivSimPlayerState.h"
#include "XivSimGameState.h"
#include "TimerManager.h"
#include "XivSimDamageType.h"

UXivSimPlayerManager::UXivSimPlayerManager()
{

}

UXivSimPlayerManager* UXivSimPlayerManager::Get(const UObject* WorldContextObject)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		if (AXivSimWorldSettings* WorldSettings = Cast<AXivSimWorldSettings>(World->GetWorldSettings()))
		{
			return WorldSettings->GetPlayerManager();
		}
	}

	return nullptr;
}

void UXivSimPlayerManager::Init()
{
	Super::Init();
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(PlayerHealingTimerHandle, this, &UXivSimPlayerManager::HealAllPlayers, PlayerHealingInterval, true);
	}
	if (AXivSimGameState* GameState = GetWorld()->GetGameState<AXivSimGameState>())
	{
		for (int32 i = 0; i < GameState->PlayerArray.Num(); ++i)
		{
			if (AXivSimPlayerState* PlayerState = Cast<AXivSimPlayerState>(GameState->PlayerArray[i]))
			{
				AddPlayerState(PlayerState);
			}
		}
	}
}

void UXivSimPlayerManager::UnInit()
{
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(PlayerHealingTimerHandle);
	}
	Super::UnInit();
}

EXivSimRolePlayType UXivSimPlayerManager::GetClassRolePlayType(EXivSimClassType InType) const
{
	if(ClassTypeToRolePlayTypeMap.Contains(InType))
	{
		return ClassTypeToRolePlayTypeMap[InType];
	}

	return EXivSimRolePlayType::RolePlay_All;
}

void UXivSimPlayerManager::AddPlayerState(class AXivSimPlayerState* InPlayerState)
{
	if (InPlayerState)
	{
		PlayerStateArray.AddUnique(InPlayerState);
		InPlayerState->OnPawnSet.AddUniqueDynamic(this, &UXivSimPlayerManager::OnPlayerPawnSet);
		if (InPlayerState->GetCurrentMarkerType() != EXivSimMarkerType::Marker_None)
		{
			TObjectPtr<AXivSimPlayerState>& PlayerStateRef = PlayerMarkerMap.FindOrAdd(InPlayerState->GetCurrentMarkerType());
			PlayerStateRef = InPlayerState;
		}

		OnPlayerStateAdd.Broadcast(InPlayerState);
		if (AXivSimCharacter* PlayerPawn = Cast<AXivSimCharacter>(InPlayerState->GetPawn()))
		{
			AddPlayer(PlayerPawn);
		}
	}
}

void UXivSimPlayerManager::RemovePlayerState(class AXivSimPlayerState* InPlayerState)
{
	if (InPlayerState)
	{
		OnPlayerStateRemove.Broadcast(InPlayerState);
		InPlayerState->OnPawnSet.RemoveDynamic(this, &UXivSimPlayerManager::OnPlayerPawnSet);
		PlayerStateArray.Remove(InPlayerState);
		
		EXivSimMarkerType KeyMarkerType = EXivSimMarkerType::Marker_None;
		for (auto It : PlayerMarkerMap)
		{
			if (It.Value == InPlayerState)
			{
				KeyMarkerType = It.Key;
			}
		}
		PlayerMarkerMap.Remove(KeyMarkerType);

		if (AXivSimCharacter* PlayerPawn = Cast<AXivSimCharacter>(InPlayerState->GetPawn()))
		{
			RemovePlayer(PlayerPawn);
		}
	}
}

void UXivSimPlayerManager::AddPlayer(class AXivSimCharacter* InPlayer)
{
	if (InPlayer)
	{
		if (PlayerList.Find(InPlayer) == INDEX_NONE)
		{
			PlayerList.Add(InPlayer);
			InPlayer->OnPlayerDead.AddUniqueDynamic(this, &UXivSimPlayerManager::OnPlayerDead);
			TArray<TWeakObjectPtr<AXivSimCharacter>>& PlayerArray = PlayerRolePlayTypeMap.FindOrAdd(InPlayer->GetRolePlayType());
			PlayerArray.Add(MakeWeakObjectPtr(InPlayer));

			if(InPlayer->GetRolePlayType() == EXivSimRolePlayType::RolePlay_Tank || InPlayer->GetRolePlayType() == EXivSimRolePlayType::RolePlay_Healer)
			{
				TArray<TWeakObjectPtr<AXivSimCharacter>>& THPlayerArray = PlayerRolePlayTypeMap.FindOrAdd(EXivSimRolePlayType::RolePlay_TankOrHealer);
				THPlayerArray.Add(MakeWeakObjectPtr(InPlayer));
			}
		}
	}
}

void UXivSimPlayerManager::RemovePlayer(class AXivSimCharacter* InPlayer)
{
	if (InPlayer)
	{
		if (PlayerList.Find(InPlayer) != INDEX_NONE)
		{
			TArray<TWeakObjectPtr<AXivSimCharacter>>* PlayerArrayPtr = PlayerRolePlayTypeMap.Find(InPlayer->GetRolePlayType());
			if (PlayerArrayPtr)
			{
				for (int32 i = PlayerArrayPtr->Num() - 1; i >= 0; --i)
				{
					if ((*PlayerArrayPtr)[i].Get() == InPlayer)
					{
						(*PlayerArrayPtr).RemoveAt(i);
						break;
					}
				}
			}

			if(InPlayer->GetRolePlayType() == EXivSimRolePlayType::RolePlay_Tank || InPlayer->GetRolePlayType() == EXivSimRolePlayType::RolePlay_Healer)
			{
				TArray<TWeakObjectPtr<AXivSimCharacter>>* THPlayerArray = PlayerRolePlayTypeMap.Find(EXivSimRolePlayType::RolePlay_TankOrHealer);
				if (THPlayerArray)
				{
					for (int32 i = THPlayerArray->Num() - 1; i >= 0; --i)
					{
						if ((*THPlayerArray)[i].Get() == InPlayer)
						{
							(*THPlayerArray).RemoveAt(i);
							break;
						}
					}
				}
			}
			
			InPlayer->OnPlayerDead.RemoveDynamic(this, &UXivSimPlayerManager::OnPlayerDead);
			PlayerList.Remove(InPlayer);
		}
	}
}

bool UXivSimPlayerManager::CheckPlayerInArena(class AXivSimCharacter* InPlayer)
{
	if (InPlayer && InPlayer->IsAlive())
	{
		if (PlayerList.Contains(InPlayer))
		{
			FVector PlayerPos = InPlayer->GetActorLocation();
			if ((PlayerPos - ArenaCenterPose).SizeSquared2D() < ArenaRadius * ArenaRadius)
			{
				return true;
			}
		}
	}
	return false;
}

TSubclassOf<class UXivSimDamageType> UXivSimPlayerManager::GetOutOfArenaDamageType() const
{
	return OutOfArenaDamageType;
}

void UXivSimPlayerManager::SetPlayerMarkerType(class AXivSimPlayerState* InPlayerState, EXivSimMarkerType MarkerType)
{
	if (InPlayerState)
	{
		if (InPlayerState->GetCurrentMarkerType() == MarkerType)
		{
			return;
		}

		if (MarkerType == EXivSimMarkerType::Marker_None)
		{
			PlayerMarkerMap.Remove(InPlayerState->GetCurrentMarkerType());
			InPlayerState->SetCurrentMarkerType(EXivSimMarkerType::Marker_None);
		}
		else
		{
			if (MarkerType == EXivSimMarkerType::Marker_Attack)
			{
				for (EXivSimMarkerType ValidType = EXivSimMarkerType::Marker_Attack1; ValidType <= EXivSimMarkerType::Marker_Attack8; )
				{
					if (!PlayerMarkerMap.Contains(ValidType))
					{
						PlayerMarkerMap.Remove(InPlayerState->GetCurrentMarkerType());
						InPlayerState->SetCurrentMarkerType(ValidType);
						PlayerMarkerMap.Add(ValidType, InPlayerState);
						break;
					}
					ValidType = EXivSimMarkerType((uint8)ValidType + 1);
				}
			}
			else if (MarkerType == EXivSimMarkerType::Marker_Chain)
			{
				for (EXivSimMarkerType ValidType = EXivSimMarkerType::Marker_Chain1; ValidType <= EXivSimMarkerType::Marker_Chain8; )
				{
					if (!PlayerMarkerMap.Contains(ValidType))
					{
						PlayerMarkerMap.Remove(InPlayerState->GetCurrentMarkerType());
						InPlayerState->SetCurrentMarkerType(ValidType);
						PlayerMarkerMap.Add(ValidType, InPlayerState);
						break;
					}
					ValidType = EXivSimMarkerType((uint8)ValidType + 1);
				}
			}
			else if (MarkerType == EXivSimMarkerType::Marker_Ignore)
			{
				for (EXivSimMarkerType ValidType = EXivSimMarkerType::Marker_Ignore1; ValidType <= EXivSimMarkerType::Marker_Ignore8; )
				{
					if (!PlayerMarkerMap.Contains(ValidType))
					{
						PlayerMarkerMap.Remove(InPlayerState->GetCurrentMarkerType());
						InPlayerState->SetCurrentMarkerType(ValidType);
						PlayerMarkerMap.Add(ValidType, InPlayerState);
						break;
					}
					ValidType = EXivSimMarkerType((uint8)ValidType + 1);
				}
			}
			else
			{
				PlayerMarkerMap.Remove(InPlayerState->GetCurrentMarkerType());
				if (PlayerMarkerMap.Contains(MarkerType))
				{
					if (PlayerMarkerMap[MarkerType])
					{
						PlayerMarkerMap[MarkerType]->SetCurrentMarkerType(EXivSimMarkerType::Marker_None);
						PlayerMarkerMap.Remove(MarkerType);
					}
				}
				InPlayerState->SetCurrentMarkerType(MarkerType);
				PlayerMarkerMap.Add(MarkerType, InPlayerState);
			}
		}
	}
}

void UXivSimPlayerManager::ClearAllPlayerMarker()
{
	for (int32 i = 0; i < PlayerStateArray.Num(); ++i)
	{
		if (PlayerStateArray[i])
		{
			PlayerStateArray[i]->SetCurrentMarkerType(EXivSimMarkerType::Marker_None);
		}
	}

	PlayerMarkerMap.Reset();
}

TArray<AXivSimCharacter*> UXivSimPlayerManager::GetRandomPlayer(int32 Count, EXivSimRolePlayType RoleType, bool bAllowDuplicate) const
{
	if (RoleType == EXivSimRolePlayType::RolePlay_All)
	{
		TArray<class AXivSimCharacter*> TempArray = PlayerList;
		return GetRandomPlayerFromList(Count, TempArray, bAllowDuplicate);
	}
	
	const TArray<TWeakObjectPtr<AXivSimCharacter>>* ArrayPtr = PlayerRolePlayTypeMap.Find(RoleType);
	if (ArrayPtr && ArrayPtr->Num() > 0)
	{
		TArray<class AXivSimCharacter*> TempArray;
		for (auto It : *ArrayPtr)
		{
			TempArray.Add(It.Get());
		}
		return GetRandomPlayerFromList(Count, TempArray, bAllowDuplicate);
	}
	
	TArray<class AXivSimCharacter*> TempArray = PlayerList;
	return GetRandomPlayerFromList(Count, TempArray, bAllowDuplicate);
}

TArray<class AXivSimCharacter*> UXivSimPlayerManager::GetRandomPlayerFromList(int32 Count, TArray<class AXivSimCharacter*>& InPlayerArray, bool bAllowDuplicate) const
{
	TArray<class AXivSimCharacter*> ResultArray;
	TArray<class AXivSimCharacter*> TempArray = InPlayerArray;
	bool bOverCount = false;
	if (InPlayerArray.Num() > 0)
	{
		while (Count > 0)
		{
			int32 RandIdx = FMath::RandRange(0, InPlayerArray.Num() - 1);
			ResultArray.Add(InPlayerArray[RandIdx]);
			InPlayerArray.RemoveAt(RandIdx);
			Count--;

			if (InPlayerArray.Num() == 0 && Count > 0)
			{
				bOverCount = true;
				if (bAllowDuplicate)
				{
					InPlayerArray = TempArray;
				}
				else
				{
					break;
				}
			}
		}
		if (bOverCount)
		{
			InPlayerArray.Reset();
		}
	}

	return ResultArray;
}

TArray<class AXivSimCharacter*> UXivSimPlayerManager::GetRandomPlayerFromList(int32 Count, const TArray<class AXivSimCharacter*>& InPlayerArray, TArray<class AXivSimCharacter*>& OutPlayerArray, bool bAllowDuplicate /*= false*/)
{
	OutPlayerArray = InPlayerArray;
	return GetRandomPlayerFromList(Count, OutPlayerArray, bAllowDuplicate);
}

TArray<class AXivSimCharacter*> UXivSimPlayerManager::GetRandomPlayerInRange(int32 Count, int32 MinCount, FTransform CenterTrans, 
	EXivSimPatternRangeType RangeType, float RangeParam1, float RangeParam2, 
	const TArray<class AXivSimCharacter*>& ExcludePlayers, bool bAllowDuplicate /*= false*/, bool bAllowOutRange /*= false*/) const
{
	TArray<class AXivSimCharacter*> ResultArray;
	switch (RangeType)
	{
	case EXivSimPatternRangeType::RangeType_All:
		ResultArray = PlayerList;
		break;
	case EXivSimPatternRangeType::RangeType_Circle:
		ResultArray = GetPlayerInCircle(CenterTrans.GetLocation(), RangeParam1);
		break;
	case EXivSimPatternRangeType::RangeType_Sector:
		ResultArray = GetPlayerInSector(CenterTrans, RangeParam1, RangeParam2);
		break;
	case EXivSimPatternRangeType::RangeType_Ring:
		ResultArray = GetPlayerInRing(CenterTrans.GetLocation(), RangeParam1, RangeParam2);
		break;
	case EXivSimPatternRangeType::RangeType_Line:
		ResultArray = GetPlayerInLine(CenterTrans, RangeParam1, RangeParam2);
		break;
	case EXivSimPatternRangeType::RangeType_HalfLine:
		ResultArray = GetPlayerInHalfLine(CenterTrans, RangeParam1, RangeParam2);
		break;
	case EXivSimPatternRangeType::RangeType_Cross:
		ResultArray = GetPlayerInCross(CenterTrans, RangeParam1, RangeParam2);
		break;
	case EXivSimPatternRangeType::RangeType_Wing:
		ResultArray = GetPlayerInWing(CenterTrans, RangeParam1, RangeParam2);
		break;
	case EXivSimPatternRangeType::RangeType_EvilEye:
		ResultArray = GetPlayerInEvilEye(CenterTrans.GetLocation(), RangeParam1, RangeParam2, false);
		break;
	case EXivSimPatternRangeType::RangeType_EvilEyeAnti:
		ResultArray = GetPlayerInEvilEye(CenterTrans.GetLocation(), RangeParam1, RangeParam2, true);
		break;
	}

	for (class AXivSimCharacter* ExcludePlayer : ExcludePlayers)
	{
		ResultArray.RemoveSingleSwap(ExcludePlayer);
	}

	TArray<class AXivSimCharacter*> TempArray = ResultArray;
	if (TempArray.Num() >= MinCount)
	{
		ResultArray = GetRandomPlayerFromList(Count, TempArray, false);
	}
	else
	{
		ResultArray = GetRandomPlayerFromList(MinCount, TempArray, bAllowDuplicate);
	}

	if (ResultArray.Num() < MinCount && bAllowOutRange)
	{
		TArray<class AXivSimCharacter*> RemainingArray = PlayerList;
		for (class AXivSimCharacter* ExistPlayer : ResultArray)
		{
			RemainingArray.RemoveSingleSwap(ExistPlayer);
		}

		for (class AXivSimCharacter* ExcludePlayer : ExcludePlayers)
		{
			RemainingArray.RemoveSingleSwap(ExcludePlayer);
		}

		if (RemainingArray.Num() > 0)
		{
			ResultArray.Append(GetRandomPlayerFromList(MinCount - ResultArray.Num(), RemainingArray, bAllowDuplicate));
		}
	}

	return ResultArray;
}

TArray<class AXivSimCharacter*> UXivSimPlayerManager::GetFarestPlayerFrom(int32 Count, FVector FromLocation) const
{
	TArray<AXivSimCharacter*> TempPlayerArray = PlayerList;
	TempPlayerArray.Sort([=](const AXivSimCharacter& Player1, const AXivSimCharacter& Player2)
		{
			float Player1Dist = (Player1.GetActorLocation() - FromLocation).Size2D();
			float Player2Dist = (Player2.GetActorLocation() - FromLocation).Size2D();

			return Player1Dist > Player2Dist;
		});

	if (TempPlayerArray.Num() <= Count)
	{
		return TempPlayerArray;
	}
	TempPlayerArray.SetNum(Count, EAllowShrinking::Yes);
	return TempPlayerArray;
}

TArray<class AXivSimCharacter*> UXivSimPlayerManager::GetNearestPlayerFrom(int32 Count, FVector FromLocation) const
{
	TArray<AXivSimCharacter*> TempPlayerArray = PlayerList;
	TempPlayerArray.Sort([=](const AXivSimCharacter& Player1, const AXivSimCharacter& Player2)
		{
			float Player1Dist = (Player1.GetActorLocation() - FromLocation).Size2D();
			float Player2Dist = (Player2.GetActorLocation() - FromLocation).Size2D();

			return Player1Dist < Player2Dist;
		});

	if (TempPlayerArray.Num() <= Count)
	{
		return TempPlayerArray;
	}
	TempPlayerArray.SetNum(Count, EAllowShrinking::Yes);
	return TempPlayerArray;
}

TArray<AXivSimCharacter*> UXivSimPlayerManager::GetFarestPlayerInListFrom(int32 Count, FVector FromLocation,
	const TArray<AXivSimCharacter*>& InPlayerArray, TArray<AXivSimCharacter*>& OutPlayerArray)
{
	TArray<AXivSimCharacter*> TempPlayerArray = InPlayerArray;
	TempPlayerArray.Sort([=](const AXivSimCharacter& Player1, const AXivSimCharacter& Player2)
		{
			float Player1Dist = (Player1.GetActorLocation() - FromLocation).Size2D();
			float Player2Dist = (Player2.GetActorLocation() - FromLocation).Size2D();

			return Player1Dist > Player2Dist;
		});

	OutPlayerArray = TArray<AXivSimCharacter*>();
	if (TempPlayerArray.Num() <= Count)
	{
		return TempPlayerArray;
	}
	for(int32 i = TempPlayerArray.Num() - 1; i >= Count; --i)
	{
		OutPlayerArray.Add(TempPlayerArray[i]);
	}
	TempPlayerArray.SetNum(Count, EAllowShrinking::Yes);
	return TempPlayerArray;
}

TArray<AXivSimCharacter*> UXivSimPlayerManager::GetNearestPlayerInListFrom(int32 Count, FVector FromLocation,
	const TArray<AXivSimCharacter*>& InPlayerArray, TArray<AXivSimCharacter*>& OutPlayerArray)
{
	TArray<AXivSimCharacter*> TempPlayerArray = InPlayerArray;
	TempPlayerArray.Sort([=](const AXivSimCharacter& Player1, const AXivSimCharacter& Player2)
		{
			float Player1Dist = (Player1.GetActorLocation() - FromLocation).Size2D();
			float Player2Dist = (Player2.GetActorLocation() - FromLocation).Size2D();

			return Player1Dist < Player2Dist;
		});

	OutPlayerArray = TArray<AXivSimCharacter*>();
	if (TempPlayerArray.Num() <= Count)
	{
		return TempPlayerArray;
	}
	for(int32 i = TempPlayerArray.Num() - 1; i >= Count; --i)
	{
		OutPlayerArray.Add(TempPlayerArray[i]);
	}
	TempPlayerArray.SetNum(Count, EAllowShrinking::Yes);
	return TempPlayerArray;
}

TArray<AXivSimCharacter*> UXivSimPlayerManager::GetFarestPlayerFromLine(int32 Count, FVector LineLocation,
	FVector LineDir) const
{
	TArray<AXivSimCharacter*> TempPlayerArray = PlayerList;
	TempPlayerArray.Sort([=](const AXivSimCharacter& Player1, const AXivSimCharacter& Player2)
		{
			float Player1Dist = FMath::PointDistToLine(Player1.GetActorLocation(), LineDir, LineLocation);
			float Player2Dist = FMath::PointDistToLine(Player2.GetActorLocation(), LineDir, LineLocation);

			return Player1Dist > Player2Dist;
		});

	if (TempPlayerArray.Num() <= Count)
	{
		return TempPlayerArray;
	}
	TempPlayerArray.SetNum(Count, EAllowShrinking::Yes);
	return TempPlayerArray;
}

TArray<AXivSimCharacter*> UXivSimPlayerManager::GetNearestPlayerFromLine(int32 Count, FVector LineLocation,
	FVector LineDir) const
{
	TArray<AXivSimCharacter*> TempPlayerArray = PlayerList;
	TempPlayerArray.Sort([=](const AXivSimCharacter& Player1, const AXivSimCharacter& Player2)
		{
			float Player1Dist = FMath::PointDistToLine(Player1.GetActorLocation(), LineDir, LineLocation);
			float Player2Dist = FMath::PointDistToLine(Player2.GetActorLocation(), LineDir, LineLocation);

			return Player1Dist < Player2Dist;
		});

	if (TempPlayerArray.Num() <= Count)
	{
		return TempPlayerArray;
	}
	TempPlayerArray.SetNum(Count, EAllowShrinking::Yes);
	return TempPlayerArray;
}

TArray<AXivSimCharacter*> UXivSimPlayerManager::GetFarestPlayerInListFromLine(int32 Count, FVector LineLocation,
	FVector LineDir, const TArray<AXivSimCharacter*>& InPlayerArray, TArray<AXivSimCharacter*>& OutPlayerArray)
{
	TArray<AXivSimCharacter*> TempPlayerArray = InPlayerArray;
	TempPlayerArray.Sort([=](const AXivSimCharacter& Player1, const AXivSimCharacter& Player2)
		{
			float Player1Dist = FMath::PointDistToLine(Player1.GetActorLocation(), LineDir, LineLocation);
			float Player2Dist = FMath::PointDistToLine(Player2.GetActorLocation(), LineDir, LineLocation);

			return Player1Dist > Player2Dist;
		});

	OutPlayerArray = TArray<AXivSimCharacter*>();
	if (TempPlayerArray.Num() <= Count)
	{
		return TempPlayerArray;
	}
	for(int32 i = TempPlayerArray.Num() - 1; i >= Count; --i)
	{
		OutPlayerArray.Add(TempPlayerArray[i]);
	}
	TempPlayerArray.SetNum(Count, EAllowShrinking::Yes);
	return TempPlayerArray;
}

TArray<AXivSimCharacter*> UXivSimPlayerManager::GetNearestPlayerInListFromLine(int32 Count, FVector LineLocation,
	FVector LineDir, const TArray<AXivSimCharacter*>& InPlayerArray, TArray<AXivSimCharacter*>& OutPlayerArray)
{
	TArray<AXivSimCharacter*> TempPlayerArray = InPlayerArray;
	TempPlayerArray.Sort([=](const AXivSimCharacter& Player1, const AXivSimCharacter& Player2)
		{
			float Player1Dist = FMath::PointDistToLine(Player1.GetActorLocation(), LineDir, LineLocation);
			float Player2Dist = FMath::PointDistToLine(Player2.GetActorLocation(), LineDir, LineLocation);

			return Player1Dist < Player2Dist;
		});

	OutPlayerArray = TArray<AXivSimCharacter*>();
	if (TempPlayerArray.Num() <= Count)
	{
		return TempPlayerArray;
	}
	for(int32 i = TempPlayerArray.Num() - 1; i >= Count; --i)
	{
		OutPlayerArray.Add(TempPlayerArray[i]);
	}
	TempPlayerArray.SetNum(Count, EAllowShrinking::Yes);
	return TempPlayerArray;
}

void UXivSimPlayerManager::OnPlayerDead(class AXivSimCharacter* PlayerPtr)
{
	if (PlayerPtr)
	{
		RemovePlayer(PlayerPtr);
	}
}

void UXivSimPlayerManager::OnPlayerPawnSet(class APlayerState* PlayerState, class APawn* NewPawn, class APawn* OldPawn)
{
	if (AXivSimCharacter* PlayerPawn = Cast<AXivSimCharacter>(OldPawn))
	{
		RemovePlayer(PlayerPawn);
	}

	if (AXivSimCharacter* PlayerPawn = Cast<AXivSimCharacter>(NewPawn))
	{
		AddPlayer(PlayerPawn);
	}
}

TArray<class AXivSimCharacter*> UXivSimPlayerManager::GetPlayerInCircle(FVector CenterLocation, float Radius) const
{
	TArray<class AXivSimCharacter*> ResultArray;
	for (AXivSimCharacter* PlayerPawn : PlayerList)
	{
		if (PlayerPawn)
		{
			if ((PlayerPawn->GetActorLocation() - CenterLocation).SizeSquared2D() <= Radius * Radius)
			{
				ResultArray.Add(PlayerPawn);
			}
		}
	}

	return ResultArray;
}

TArray<class AXivSimCharacter*> UXivSimPlayerManager::GetPlayerInSector(FTransform CenterTrans, float Radius, float Degree) const
{
	TArray<class AXivSimCharacter*> ResultArray;
	FVector2D ForwardVec = FVector2D(CenterTrans.GetRotation().GetForwardVector());
	FVector2D PlayerVec;
	float DotValue = 0.f;
	for (AXivSimCharacter* PlayerPawn : PlayerList)
	{
		if (PlayerPawn)
		{
			PlayerVec = FVector2D(PlayerPawn->GetActorLocation() - CenterTrans.GetLocation());
			if (PlayerVec.SizeSquared() <= Radius * Radius)
			{
				DotValue = PlayerVec.GetSafeNormal().Dot(ForwardVec);
				if (DotValue > 0.f && DotValue > FMath::Cos(UE_DOUBLE_PI / (360.0) * Degree))
				{
					ResultArray.Add(PlayerPawn);
				}
			}
		}
	}
	return ResultArray;
}

TArray<class AXivSimCharacter*> UXivSimPlayerManager::GetPlayerInRing(FVector CenterLocation, float Radius, float InnerRadius) const
{
	TArray<class AXivSimCharacter*> ResultArray;
	float PlayerDisSquared = 0.f;
	for (AXivSimCharacter* PlayerPawn : PlayerList)
	{
		if (PlayerPawn)
		{
			PlayerDisSquared = (PlayerPawn->GetActorLocation() - CenterLocation).SizeSquared2D();
			if (PlayerDisSquared <= Radius * Radius && PlayerDisSquared > InnerRadius * InnerRadius)
			{
				ResultArray.Add(PlayerPawn);
			}
		}
	}

	return ResultArray;
}

TArray<class AXivSimCharacter*> UXivSimPlayerManager::GetPlayerInLine(FTransform CenterTrans, float Length, float Width) const
{
	TArray<class AXivSimCharacter*> ResultArray;
	FVector RelativeLocation;
	for (AXivSimCharacter* PlayerPawn : PlayerList)
	{
		if (PlayerPawn)
		{
			RelativeLocation = CenterTrans.InverseTransformPosition(PlayerPawn->GetActorLocation());
			if (FMath::Abs(RelativeLocation.X) <= Length / 2 && FMath::Abs(RelativeLocation.Y) <= Width / 2)
			{
				ResultArray.Add(PlayerPawn);
			}
		}
	}

	return ResultArray;
}

TArray<class AXivSimCharacter*> UXivSimPlayerManager::GetPlayerInHalfLine(FTransform CenterTrans, float Length, float Width) const
{
	TArray<class AXivSimCharacter*> ResultArray;
	FVector RelativeLocation;
	for (AXivSimCharacter* PlayerPawn : PlayerList)
	{
		if (PlayerPawn)
		{
			RelativeLocation = CenterTrans.InverseTransformPosition(PlayerPawn->GetActorLocation());
			if (RelativeLocation.X > 0.f && RelativeLocation.X <= Length && FMath::Abs(RelativeLocation.Y) <= Width / 2)
			{
				ResultArray.Add(PlayerPawn);
			}
		}
	}

	return ResultArray;
}

TArray<class AXivSimCharacter*> UXivSimPlayerManager::GetPlayerInCross(FTransform CenterTrans, float Length, float Width) const
{
	TArray<class AXivSimCharacter*> ResultArray;
	FVector RelativeLocation;
	for (AXivSimCharacter* PlayerPawn : PlayerList)
	{
		if (PlayerPawn)
		{
			RelativeLocation = CenterTrans.InverseTransformPosition(PlayerPawn->GetActorLocation());
			if ((FMath::Abs(RelativeLocation.X) <= Length / 2 && FMath::Abs(RelativeLocation.Y) <= Width / 2) ||
				(FMath::Abs(RelativeLocation.Y) <= Length / 2 && FMath::Abs(RelativeLocation.X) <= Width / 2))
			{
				ResultArray.Add(PlayerPawn);
			}
		}
	}

	return ResultArray;
}

TArray<class AXivSimCharacter*> UXivSimPlayerManager::GetPlayerInWing(FTransform CenterTrans, float Length, float Width) const
{
	TArray<class AXivSimCharacter*> ResultArray;
	FVector RelativeLocation;
	for (AXivSimCharacter* PlayerPawn : PlayerList)
	{
		if (PlayerPawn)
		{
			RelativeLocation = CenterTrans.InverseTransformPosition(PlayerPawn->GetActorLocation());
			if (FMath::Abs(RelativeLocation.Y) > Width / 2 && 
				FMath::Abs(RelativeLocation.Y) <= Length / 2 && 
				FMath::Abs(RelativeLocation.X) <= Length / 2)
			{
				ResultArray.Add(PlayerPawn);
			}
		}
	}

	return ResultArray;
}

TArray<AXivSimCharacter*> UXivSimPlayerManager::GetPlayerInEvilEye(FVector CenterLocation, float Radius,
	float Degree, bool bAnti) const
{
	TArray<class AXivSimCharacter*> ResultArray;
	for (AXivSimCharacter* PlayerPawn : PlayerList)
	{
		if (PlayerPawn)
		{
			FVector PlayerTargetVector = CenterLocation - PlayerPawn->GetActorLocation();
			if (PlayerTargetVector.SizeSquared2D() <= Radius * Radius)
			{
				FVector PlayerForwardVector = PlayerPawn->GetActorForwardVector();
				PlayerTargetVector.Z = 0;
				PlayerForwardVector.Z = 0;
				float AngleDot = FVector::DotProduct(PlayerForwardVector.GetSafeNormal(), PlayerTargetVector.GetSafeNormal());
				float AngleAcos = acosf(AngleDot);
				float TestDegree = FMath::RadiansToDegrees(AngleAcos);
				if((!bAnti && TestDegree <= Degree) || (bAnti && TestDegree > Degree))
				{
					ResultArray.Add(PlayerPawn);
				}
			}
		}
	}

	return ResultArray;
}

void UXivSimPlayerManager::HealAllPlayers()
{
	for (AXivSimCharacter* Player : PlayerList)
	{
		if (Player && Player->IsAlive())
		{
			Player->SetHealth(Player->GetHealth() + PlayerHealingAmout);
		}
	}
}

