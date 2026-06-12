#pragma once

#include "AIController.h"
#include "XivSimBotController.generated.h"

UCLASS()
class AXivSimBotController : public AAIController
{
	GENERATED_BODY()

public:
	AXivSimBotController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};