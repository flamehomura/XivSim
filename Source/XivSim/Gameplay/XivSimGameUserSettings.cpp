#include "XivSimGameUserSettings.h"

#define XS_GAMEUSERSETTINGS_VERSION  9

UXivSimGameUserSettings::UXivSimGameUserSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUseStandardInputMode = true;
	LocalPlayerName = TEXT("FFXiv Player");
	LocalPlayerClassType = EXivSimClassType::Class_Warrior;
	LocalPartyListPositiveSort = true;
	LocalPartyListLocalFirst = true;
	MarkerAWorldPosition = FVector2D(1000.f, 0.f);
	MarkerBWorldPosition = FVector2D(0.f, 1000.f);
	MarkerCWorldPosition = FVector2D(-1000.f, 0.f);
	MarkerDWorldPosition = FVector2D(0.f, -1000.f);
	Marker1WorldPosition = FVector2D(707.f, 707.f);
	Marker2WorldPosition = FVector2D(-707.f, 707.f);
	Marker3WorldPosition = FVector2D(-707.f, -707.f);
	Marker4WorldPosition = FVector2D(707.f, -707.f);
}

UXivSimGameUserSettings* UXivSimGameUserSettings::GetXivSimGameUserSettings()
{
	return Cast<UXivSimGameUserSettings>(UGameUserSettings::GetGameUserSettings());
}

void UXivSimGameUserSettings::SetUseStandardInputMode(const bool& bNewMode)
{
	bUseStandardInputMode = bNewMode;
	SaveSettings();
}

const bool& UXivSimGameUserSettings::GetUseStandardInputMode() const
{
	return bUseStandardInputMode;
}

void UXivSimGameUserSettings::SetLocalPlayerName(const FString& NewName)
{
	LocalPlayerName = NewName;
	SaveSettings();
}

const FString& UXivSimGameUserSettings::GetLocalPlayerName() const
{
	return LocalPlayerName;
}

void UXivSimGameUserSettings::SetLocalPlayerClassType(const EXivSimClassType& NewType)
{
	LocalPlayerClassType = NewType;
	SaveSettings();
}

const EXivSimClassType& UXivSimGameUserSettings::GetLocalPlayerClassType() const
{
	return LocalPlayerClassType;
}

void UXivSimGameUserSettings::SetLocalPlayerPositionType(const EXivSimPositionType& NewType)
{
	LocalPlayerPositionType = NewType;
	SaveSettings();
}

const EXivSimPositionType& UXivSimGameUserSettings::GetLocalPlayerPositionType() const
{
	return LocalPlayerPositionType;
}

void UXivSimGameUserSettings::SetLocalPartyListPositiveSort(const bool& bPositive)
{
	LocalPartyListPositiveSort = bPositive;
	SaveSettings();
}

const bool& UXivSimGameUserSettings::GetLocalPartyListPositiveSort() const
{
	return LocalPartyListPositiveSort;
}

void UXivSimGameUserSettings::SetLocalPartyListLocalFirst(const bool& bFirst)
{
	LocalPartyListLocalFirst = bFirst;
	SaveSettings();
}

const bool& UXivSimGameUserSettings::GetLocalPartyListLocalFirst() const
{
	return LocalPartyListLocalFirst;
}

void UXivSimGameUserSettings::SetMarkerAWorldPosition(const FVector2D& NewPos)
{
	MarkerAWorldPosition = NewPos;
	SaveSettings();
}

const FVector2D& UXivSimGameUserSettings::GetMarkerAWorldPosition() const
{
	return MarkerAWorldPosition;
}

void UXivSimGameUserSettings::SetMarkerBWorldPosition(const FVector2D& NewPos)
{
	MarkerBWorldPosition = NewPos;
	SaveSettings();
}

const FVector2D& UXivSimGameUserSettings::GetMarkerBWorldPosition() const
{
	return MarkerBWorldPosition;
}

void UXivSimGameUserSettings::SetMarkerCWorldPosition(const FVector2D& NewPos)
{
	MarkerCWorldPosition = NewPos;
	SaveSettings();
}

const FVector2D& UXivSimGameUserSettings::GetMarkerCWorldPosition() const
{
	return MarkerCWorldPosition;
}

void UXivSimGameUserSettings::SetMarkerDWorldPosition(const FVector2D& NewPos)
{
	MarkerDWorldPosition = NewPos;
	SaveSettings();
}

const FVector2D& UXivSimGameUserSettings::GetMarkerDWorldPosition() const
{
	return MarkerDWorldPosition;
}

void UXivSimGameUserSettings::SetMarker1WorldPosition(const FVector2D& NewPos)
{
	Marker1WorldPosition = NewPos;
	SaveSettings();
}

const FVector2D& UXivSimGameUserSettings::GetMarker1WorldPosition() const
{
	return Marker1WorldPosition;
}

void UXivSimGameUserSettings::SetMarker2WorldPosition(const FVector2D& NewPos)
{
	Marker2WorldPosition = NewPos;
	SaveSettings();
}

const FVector2D& UXivSimGameUserSettings::GetMarker2WorldPosition() const
{
	return Marker2WorldPosition;
}

void UXivSimGameUserSettings::SetMarker3WorldPosition(const FVector2D& NewPos)
{
	Marker3WorldPosition = NewPos;
	SaveSettings();
}

const FVector2D& UXivSimGameUserSettings::GetMarker3WorldPosition() const
{
	return Marker3WorldPosition;
}

void UXivSimGameUserSettings::SetMarker4WorldPosition(const FVector2D& NewPos)
{
	Marker4WorldPosition = NewPos;
	SaveSettings();
}

const FVector2D& UXivSimGameUserSettings::GetMarker4WorldPosition() const
{
	return Marker4WorldPosition;
}
