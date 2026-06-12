#include "XivSimWorldSettings.h"

#include "XivSimActionManager.h"
#include "XivSimPlayerManager.h"
#include "XivSimPatternManager.h"

AXivSimWorldSettings::AXivSimWorldSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AXivSimWorldSettings::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();
}

void AXivSimWorldSettings::BeginPlay()
{
	Super::BeginPlay();
	CreateManagers();
}

void AXivSimWorldSettings::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroyManagers();
	Super::EndPlay(EndPlayReason);
}

void AXivSimWorldSettings::CreateManagers()
{
	if (PlayerManagerClass)
	{
		PlayerManager = NewObject<UXivSimPlayerManager>(this, PlayerManagerClass);
	}
	else
	{
		PlayerManager = NewObject<UXivSimPlayerManager>(this, UXivSimPlayerManager::StaticClass());
	}
	PlayerManager->Init();

	if (PatternManagerClass)
	{
		PatternManager = NewObject<UXivSimPatternManager>(this, PatternManagerClass);
	}
	else
	{
		PatternManager = NewObject<UXivSimPatternManager>(this, UXivSimPatternManager::StaticClass());
	}
	PatternManager->Init();

	if(ActionManagerClass)
	{
		ActionManager = NewObject<UXivSimActionManager>(this, ActionManagerClass);
	}
	else
	{
		ActionManager = NewObject<UXivSimActionManager>(this, UXivSimActionManager::StaticClass());
	}
	ActionManager->Init();
}

void AXivSimWorldSettings::DestroyManagers()
{
	if (PlayerManager)
	{
		PlayerManager->UnInit();
		PlayerManager = nullptr;
	}

	if (PatternManager)
	{
		PatternManager->UnInit();
		PatternManager = nullptr;
	}
}
