#include "XivSimPatternManager.h"
#include "XivSimWorldSettings.h"
#include "Engine/DataTable.h"
#include "XivSimPatternBase.h"
#include "XivSimGameState.h"

UXivSimPatternManager::UXivSimPatternManager()
{

}

UXivSimPatternManager* UXivSimPatternManager::Get(const UObject* WorldContextObject)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		if (AXivSimWorldSettings* WorldSettings = Cast<AXivSimWorldSettings>(World->GetWorldSettings()))
		{
			return WorldSettings->GetPatternManager();
		}
	}

	return nullptr;
}

void UXivSimPatternManager::Init()
{
	Super::Init();

	PatternDataMap.Reset();
	if (PatternDataTable)
	{
		for (auto It : PatternDataTable->GetRowMap())
		{
			FXivSimPatternData* InData = reinterpret_cast<FXivSimPatternData*>(It.Value);
			if (InData)
			{
				PatternDataMap.Add(InData->PatternID, *InData);
			}
		}
	}
}

void UXivSimPatternManager::ActivatePatternAtLocation(int32 PatternID, FTransform PatternTrans, AActor* TargetActor)
{
	if (AXivSimGameState* GameState = GetWorld()->GetGameState<AXivSimGameState>())
	{
		UXivSimPatternBase* NewPattern = CreatePattern(PatternID, GameState);
		if (NewPattern)
		{
			NewPattern->SetPatternTransform(PatternTrans);
			NewPattern->SetPatternTarget(TargetActor);
			GameState->AddPattern(NewPattern);
		}
	}
}

void UXivSimPatternManager::ActivatePatternAttach(int32 PatternID, AActor* AttachParent, AActor* TargetActor)
{
	if (AttachParent)
	{
		if (AXivSimGameState* GameState = GetWorld()->GetGameState<AXivSimGameState>())
		{
			UXivSimPatternBase* NewPattern = CreatePattern(PatternID, GameState);
			if (NewPattern)
			{
				NewPattern->SetPatternOwner(AttachParent);
				NewPattern->SetPatternTarget(TargetActor);
				GameState->AddPattern(NewPattern);
			}
		}
	}
}

void UXivSimPatternManager::ActivatePatternAtLocationExclude(int32 PatternID, FTransform PatternTrans, const TArray<class AXivSimCharacter*>& ExcludePlayers, AActor* TargetActor /*= nullptr*/)
{
	if (AXivSimGameState* GameState = GetWorld()->GetGameState<AXivSimGameState>())
	{
		UXivSimPatternBase* NewPattern = CreatePattern(PatternID, GameState);
		if (NewPattern)
		{
			NewPattern->SetPatternTransform(PatternTrans);
			NewPattern->SetPatternTarget(TargetActor);
			NewPattern->SetExcludePlayerList(ExcludePlayers);
			GameState->AddPattern(NewPattern);
		}
	}
}

void UXivSimPatternManager::ActivatePatternAttachExclude(int32 PatternID, AActor* AttachParent, const TArray<class AXivSimCharacter*>& ExcludePlayers, AActor* TargetActor /*= nullptr*/)
{
	if (AttachParent)
	{
		if (AXivSimGameState* GameState = GetWorld()->GetGameState<AXivSimGameState>())
		{
			UXivSimPatternBase* NewPattern = CreatePattern(PatternID, GameState);
			if (NewPattern)
			{
				NewPattern->SetPatternOwner(AttachParent);
				NewPattern->SetPatternTarget(TargetActor);
				NewPattern->SetExcludePlayerList(ExcludePlayers);
				GameState->AddPattern(NewPattern);
			}
		}
	}
}

FXivSimPatternData* UXivSimPatternManager::GetPatternData(int32 PatternID)
{
	return PatternDataMap.Find(PatternID);
}

class UXivSimPatternBase* UXivSimPatternManager::CreatePattern(int32 PatternID, UObject* OwnerObj)
{
	UXivSimPatternBase* NewPattern = nullptr;
	if (FXivSimPatternData* PatternData = PatternDataMap.Find(PatternID))
	{
		if (PatternData->PatternClass)
		{
			NewPattern = NewObject<UXivSimPatternBase>(OwnerObj, PatternData->PatternClass);
		}
		else
		{
			NewPattern = NewObject<UXivSimPatternBase>(OwnerObj, UXivSimPatternBase::StaticClass());
		}

		if (NewPattern)
		{
			NewPattern->SetPatternID(PatternID);
		}
	}

	return NewPattern;
}

