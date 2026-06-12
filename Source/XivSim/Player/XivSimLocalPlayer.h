#pragma once

#include "Engine/LocalPlayer.h"
#include "XivSimLocalPlayer.generated.h"

UCLASS()
class UXivSimLocalPlayer : public ULocalPlayer
{
	GENERATED_UCLASS_BODY()
	
public:
	void SetPlayerNickname(FString NewName);
	virtual FString GetNickname() const override;

protected:
	UPROPERTY()
	FString CustomNickname;
};