#pragma once

#include "GameFramework/CheatManager.h"
#include "XivSimCheatManager.generated.h"

UCLASS()
class UXivSimCheatManager : public UCheatManager
{
	GENERATED_UCLASS_BODY()

	virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;

	UFUNCTION(exec, BlueprintCallable, Category = "Cheat Manager")
	virtual void AllGod();

	UFUNCTION(exec, BlueprintCallable, Category = "Cheat Manager")
	virtual void Training();
};