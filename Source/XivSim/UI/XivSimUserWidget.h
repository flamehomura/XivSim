#pragma once

#include "Blueprint/UserWidget.h"
#include "XivSimUserWidget.generated.h"

UCLASS()
class UXivSimUserWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()

protected:
	class AXivSimPlayerController* GetOwningXivSimPC() const;
	class AXivSimCharacter* GetOwningXivSimPlayer() const;
};