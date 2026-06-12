// Copyright Epic Games, Inc. All Rights Reserved.

#include "XivSimGameMode.h"
#include "XivSimCharacter.h"
#include "UObject/ConstructorHelpers.h"

AXivSimGameMode::AXivSimGameMode()
{
}

bool AXivSimGameMode::AllowCheats(APlayerController* P)
{
	return true;
}
