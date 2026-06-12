#include "XivSimLocalPlayer.h"

UXivSimLocalPlayer::UXivSimLocalPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UXivSimLocalPlayer::SetPlayerNickname(FString NewName)
{
	CustomNickname = NewName;
}

FString UXivSimLocalPlayer::GetNickname() const
{
	if (CustomNickname != "")
	{
		return CustomNickname;
	}

	return Super::GetNickname();
}

