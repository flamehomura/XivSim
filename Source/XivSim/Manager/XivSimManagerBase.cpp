#include "XivSimManagerBase.h"
#include "XivSimWorldSettings.h"

UXivSimManagerBase::UXivSimManagerBase()
{

}

void UXivSimManagerBase::Init()
{

}

void UXivSimManagerBase::UnInit()
{

}

bool UXivSimManagerBase::HasAuthority()
{
	if (AXivSimWorldSettings* WorldSettings = Cast<AXivSimWorldSettings>(GetOuter()))
	{
		return WorldSettings->HasAuthority();
	}

	return false;
}

