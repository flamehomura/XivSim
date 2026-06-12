// Copyright@Homura Akane@chocobo

#pragma once

#include "CoreMinimal.h"
#include "XivSimManagerBase.h"
#include "XivSim.h"
#include "XivSimActionManager.generated.h"


class UDataTable;
class UXivSimActionBase;
/**
 * 
 */
UCLASS()
class UXivSimActionManager : public UXivSimManagerBase
{
	GENERATED_BODY()

public:
	UXivSimActionManager();
	
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetActionManager", DefaultToSelf = "WorldContextObject"))
	static UXivSimActionManager* Get(const UObject* WorldContextObject);
	
	virtual void Init() override;
	
	const FXivSimActionData* GetActionData(int32 InActionID) const;
	class UXivSimActionBase* GetActionObject(int32 InActionID);
	TArray<FXivSimActionData> GetClassActionDatas(EXivSimClassType InClassType);

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UDataTable> ActionDataTable;

	UPROPERTY()
	TMap<int32, FXivSimActionData> ActionDataMap;
	TMap<uint32, TArray<FXivSimActionData>> ClassActionDataMap;
	UPROPERTY()
	TMap<int32, TObjectPtr<UXivSimActionBase>> ActionObjectMap;
};
