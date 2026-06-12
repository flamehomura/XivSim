// Copyright@Homura Akane@Chocobo


#include "XivSimActionManager.h"

#include "XivSimActionBase.h"
#include "XivSimWorldSettings.h"

UXivSimActionManager::UXivSimActionManager()
{
}

UXivSimActionManager* UXivSimActionManager::Get(const UObject* WorldContextObject)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		if (AXivSimWorldSettings* WorldSettings = Cast<AXivSimWorldSettings>(World->GetWorldSettings()))
		{
			return WorldSettings->GetActionManager();
		}
	}

	return nullptr;
}

void UXivSimActionManager::Init()
{
	Super::Init();
	
	ActionDataMap.Reset();
	ClassActionDataMap.Reset();
	ActionObjectMap.Reset();
	if (ActionDataTable)
	{
		for (auto It : ActionDataTable->GetRowMap())
		{
			FXivSimActionData* InData = reinterpret_cast<FXivSimActionData*>(It.Value);
			if (InData)
			{
				ActionDataMap.Add(InData->ActionID, *InData);

				UXivSimActionBase* ActionObj = nullptr;
				if(InData->ActionClass)
				{
					ActionObj = NewObject<UXivSimActionBase>(this, InData->ActionClass);
				}
				else
				{
					ActionObj = NewObject<UXivSimActionBase>(this, UXivSimActionBase::StaticClass());
				}
				if(ActionObj)
				{
					ActionObj->SetActionData(*InData);
					ActionObjectMap.Add(InData->ActionID, ActionObj);
				}

				if(TArray<FXivSimActionData>* ClassActionArray = ClassActionDataMap.Find(InData->ClassTypeBitMask))
				{
					ClassActionArray->Add(*InData);
				}
				else
				{
					TArray<FXivSimActionData> NewArray;
					NewArray.Add(*InData);
					ClassActionDataMap.Add(InData->ClassTypeBitMask, NewArray);
				}
			}
		}
	}
}

const FXivSimActionData* UXivSimActionManager::GetActionData(int32 InActionID) const
{
	return ActionDataMap.Find(InActionID);
}

UXivSimActionBase* UXivSimActionManager::GetActionObject(int32 InActionID)
{
	if(TObjectPtr<UXivSimActionBase>* ObjPtr = ActionObjectMap.Find(InActionID))
	{
		return *ObjPtr;
	}

	return nullptr;
}

TArray<FXivSimActionData> UXivSimActionManager::GetClassActionDatas(EXivSimClassType InClassType)
{
	TArray<FXivSimActionData> ResArray;
	TArray<uint32> ClassKeys;
	ClassActionDataMap.GetKeys(ClassKeys);
	for(const uint32& ClassBits : ClassKeys)
	{
		if(ClassBits & (1 << (static_cast<int32>(InClassType) - 1)))
		{
			if(const TArray<FXivSimActionData>* FindArray = ClassActionDataMap.Find(ClassBits))
			{
				ResArray.Append(*FindArray);
			}
		}
	}

	return ResArray;
}
