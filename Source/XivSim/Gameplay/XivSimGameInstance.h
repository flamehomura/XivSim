// Copyright@Homura Akane@Chocobo

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "XivSimGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class XIVSIM_API UXivSimGameInstance : public UGameInstance
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void LoadMapMarkerPositions();
	
	UFUNCTION(BlueprintCallable)
	void SetMapMarkerPosition(FName MarkerTag, FVector2D NewPos);
};
