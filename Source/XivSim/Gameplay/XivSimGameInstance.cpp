// Copyright@Homura Akane@Chocobo


#include "XivSimGameInstance.h"
#include "XivSimGameUserSettings.h"
#include "EngineUtils.h"

class UXivSimGameUserSettings;

UXivSimGameInstance::UXivSimGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UXivSimGameInstance::LoadMapMarkerPositions()
{
	if(UXivSimGameUserSettings* GUS = UXivSimGameUserSettings::GetXivSimGameUserSettings())
	{
		if (GetWorld())
		{
			for(FActorIterator It(GetWorld()); It; ++It)
			{
				AActor* Actor = *It;
				if(Actor->ActorHasTag(FName("MarkerA")))
				{
					Actor->SetActorLocation(FVector(GUS->GetMarkerAWorldPosition(), 0.f));
				}
				else if(Actor->ActorHasTag(FName("MarkerB")))
				{
					Actor->SetActorLocation(FVector(GUS->GetMarkerBWorldPosition(), 0.f));
				}
				else if(Actor->ActorHasTag(FName("MarkerC")))
				{
					Actor->SetActorLocation(FVector(GUS->GetMarkerCWorldPosition(), 0.f));
				}
				else if(Actor->ActorHasTag(FName("MarkerD")))
				{
					Actor->SetActorLocation(FVector(GUS->GetMarkerDWorldPosition(), 0.f));
				}
				else if(Actor->ActorHasTag(FName("Marker1")))
				{
					Actor->SetActorLocation(FVector(GUS->GetMarker1WorldPosition(), 0.f));
				}
				else if(Actor->ActorHasTag(FName("Marker2")))
				{
					Actor->SetActorLocation(FVector(GUS->GetMarker2WorldPosition(), 0.f));
				}
				else if(Actor->ActorHasTag(FName("Marker3")))
				{
					Actor->SetActorLocation(FVector(GUS->GetMarker3WorldPosition(), 0.f));
				}
				else if(Actor->ActorHasTag(FName("Marker4")))
				{
					Actor->SetActorLocation(FVector(GUS->GetMarker4WorldPosition(), 0.f));
				}
			}
		}
	}
}

void UXivSimGameInstance::SetMapMarkerPosition(FName MarkerTag, FVector2D NewPos)
{
	if (GetWorld())
	{
		for(FActorIterator It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;
			if(Actor->ActorHasTag(MarkerTag))
			{
				Actor->SetActorLocation(FVector(NewPos, 0.f));
			}
		}
		
		if(UXivSimGameUserSettings* GUS = UXivSimGameUserSettings::GetXivSimGameUserSettings())
		{
			if(MarkerTag == FName("MarkerA"))
			{
				GUS->SetMarkerAWorldPosition(NewPos);	
			}
			else if(MarkerTag == FName("MarkerB"))
			{
				GUS->SetMarkerBWorldPosition(NewPos);	
			}
			else if(MarkerTag == FName("MarkerC"))
			{
				GUS->SetMarkerCWorldPosition(NewPos);	
			}
			else if(MarkerTag == FName("MarkerD"))
			{
				GUS->SetMarkerDWorldPosition(NewPos);	
			}
			else if(MarkerTag == FName("Marker1"))
			{
				GUS->SetMarker1WorldPosition(NewPos);	
			}
			else if(MarkerTag == FName("Marker2"))
			{
				GUS->SetMarker2WorldPosition(NewPos);	
			}
			else if(MarkerTag == FName("Marker3"))
			{
				GUS->SetMarker3WorldPosition(NewPos);	
			}
			else if(MarkerTag == FName("Marker4"))
			{
				GUS->SetMarker4WorldPosition(NewPos);	
			}
		}
	}
}
