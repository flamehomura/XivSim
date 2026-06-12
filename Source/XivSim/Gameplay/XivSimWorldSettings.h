#pragma once

#include "GameFramework/WorldSettings.h"
#include "XivSimWorldSettings.generated.h"

class UXivSimPlayerManager;
class UXivSimPatternManager;
class UXivSimActionManager;

UCLASS()
class AXivSimWorldSettings : public AWorldSettings
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void PostRegisterAllComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintPure)
	UXivSimPlayerManager* GetPlayerManager() { return PlayerManager; }

	UFUNCTION(BlueprintPure)
	UXivSimPatternManager* GetPatternManager() { return PatternManager; }
	
	UFUNCTION(BlueprintPure)
	UXivSimActionManager* GetActionManager() { return ActionManager; }

protected:
	void CreateManagers();
	void DestroyManagers();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Manager")
	TSubclassOf<UXivSimPlayerManager> PlayerManagerClass;
	
	UPROPERTY(Transient)
	TObjectPtr<UXivSimPlayerManager> PlayerManager;

	UPROPERTY(EditDefaultsOnly, Category = "Manager")
	TSubclassOf<UXivSimPatternManager> PatternManagerClass;

	UPROPERTY(Transient)
	TObjectPtr<UXivSimPatternManager> PatternManager;
	
	UPROPERTY(EditDefaultsOnly, Category = "Manager")
	TSubclassOf<UXivSimActionManager> ActionManagerClass;
	
	UPROPERTY(Transient)
	TObjectPtr<UXivSimActionManager> ActionManager;
};