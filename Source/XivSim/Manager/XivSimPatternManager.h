#pragma once

#include "XivSimManagerBase.h"
#include "XivSim.h"
#include "XivSimPatternManager.generated.h"

class UDataTable;

UCLASS()
class UXivSimPatternManager : public UXivSimManagerBase
{
	GENERATED_BODY()

public:
	UXivSimPatternManager();
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetPatternManager", DefaultToSelf = "WorldContextObject"))
	static UXivSimPatternManager* Get(const UObject* WorldContextObject);

	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	void ActivatePatternAtLocation(int32 PatternID, FTransform PatternTrans, AActor* TargetActor = nullptr);

	UFUNCTION(BlueprintCallable)
	void ActivatePatternAttach(int32 PatternID, AActor* AttachParent, AActor* TargetActor = nullptr);

	UFUNCTION(BlueprintCallable)
	void ActivatePatternAtLocationExclude(int32 PatternID, FTransform PatternTrans, const TArray<class AXivSimCharacter*>& ExcludePlayers, AActor* TargetActor = nullptr);

	UFUNCTION(BlueprintCallable)
	void ActivatePatternAttachExclude(int32 PatternID, AActor* AttachParent, const TArray<class AXivSimCharacter*>& ExcludePlayers, AActor* TargetActor = nullptr);

	FXivSimPatternData* GetPatternData(int32 PatternID);

protected:
	class UXivSimPatternBase* CreatePattern(int32 PatternID, UObject* OwnerObj = nullptr);

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UDataTable> PatternDataTable;

	UPROPERTY()
	TMap<int32, FXivSimPatternData> PatternDataMap;
};