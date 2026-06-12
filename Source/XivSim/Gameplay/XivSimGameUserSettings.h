#pragma once

#include "CoreMinimal.h"
#include "XivSim.h"
#include "GameFramework/GameUserSettings.h"
#include "XivSimGameUserSettings.generated.h"

UCLASS()
class UXivSimGameUserSettings : public UGameUserSettings
{
	GENERATED_UCLASS_BODY()
	
public:
	UFUNCTION(BlueprintPure)
	static UXivSimGameUserSettings* GetXivSimGameUserSettings();
	
	UFUNCTION(BlueprintCallable)
	void SetUseStandardInputMode(const bool& bNewMode);
	UFUNCTION(BlueprintPure)
	const bool& GetUseStandardInputMode() const;

	UFUNCTION(BlueprintCallable)
	void SetLocalPlayerName(const FString& NewName);
	UFUNCTION(BlueprintPure)
	const FString& GetLocalPlayerName() const;

	UFUNCTION(BlueprintCallable)
	void SetLocalPlayerClassType(const EXivSimClassType& NewType);
	UFUNCTION(BlueprintPure)
	const EXivSimClassType& GetLocalPlayerClassType() const;
	
	UFUNCTION(BlueprintCallable)
	void SetLocalPlayerPositionType(const EXivSimPositionType& NewType);
	UFUNCTION(BlueprintPure)
	const EXivSimPositionType& GetLocalPlayerPositionType() const;

	UFUNCTION(BlueprintCallable)
	void SetLocalPartyListPositiveSort(const bool& bPositive);
	UFUNCTION(BlueprintPure)
	const bool& GetLocalPartyListPositiveSort() const;
	
	UFUNCTION(BlueprintCallable)
	void SetLocalPartyListLocalFirst(const bool& bFirst);
	UFUNCTION(BlueprintPure)
	const bool& GetLocalPartyListLocalFirst() const;

	UFUNCTION(BlueprintCallable)
	void SetMarkerAWorldPosition(const FVector2D& NewPos);
	UFUNCTION(BlueprintPure)
	const FVector2D& GetMarkerAWorldPosition() const;
	
	UFUNCTION(BlueprintCallable)
	void SetMarkerBWorldPosition(const FVector2D& NewPos);
	UFUNCTION(BlueprintPure)
	const FVector2D& GetMarkerBWorldPosition() const;
	
	UFUNCTION(BlueprintCallable)
	void SetMarkerCWorldPosition(const FVector2D& NewPos);
	UFUNCTION(BlueprintPure)
	const FVector2D& GetMarkerCWorldPosition() const;
	
	UFUNCTION(BlueprintCallable)
	void SetMarkerDWorldPosition(const FVector2D& NewPos);
	UFUNCTION(BlueprintPure)
	const FVector2D& GetMarkerDWorldPosition() const;
	
	UFUNCTION(BlueprintCallable)
	void SetMarker1WorldPosition(const FVector2D& NewPos);
	UFUNCTION(BlueprintPure)
	const FVector2D& GetMarker1WorldPosition() const;
	
	UFUNCTION(BlueprintCallable)
	void SetMarker2WorldPosition(const FVector2D& NewPos);
	UFUNCTION(BlueprintPure)
	const FVector2D& GetMarker2WorldPosition() const;
	
	UFUNCTION(BlueprintCallable)
	void SetMarker3WorldPosition(const FVector2D& NewPos);
	UFUNCTION(BlueprintPure)
	const FVector2D& GetMarker3WorldPosition() const;
	
	UFUNCTION(BlueprintCallable)
	void SetMarker4WorldPosition(const FVector2D& NewPos);
	UFUNCTION(BlueprintPure)
	const FVector2D& GetMarker4WorldPosition() const;

protected:
	UPROPERTY(Config)
	bool bUseStandardInputMode;

	UPROPERTY(Config)
	FString LocalPlayerName;
	
	UPROPERTY(Config)
	EXivSimClassType LocalPlayerClassType;
	
	UPROPERTY(Config)
	EXivSimPositionType LocalPlayerPositionType;
	
	UPROPERTY(Config)
	bool LocalPartyListPositiveSort;
	
	UPROPERTY(Config)
	bool LocalPartyListLocalFirst;
	
	UPROPERTY(Config)
	FVector2D MarkerAWorldPosition;
	
	UPROPERTY(Config)
	FVector2D MarkerBWorldPosition;
	
	UPROPERTY(Config)
	FVector2D MarkerCWorldPosition;
	
	UPROPERTY(Config)
	FVector2D MarkerDWorldPosition;
	
	UPROPERTY(Config)
	FVector2D Marker1WorldPosition;
	
	UPROPERTY(Config)
	FVector2D Marker2WorldPosition;
	
	UPROPERTY(Config)
	FVector2D Marker3WorldPosition;
	
	UPROPERTY(Config)
	FVector2D Marker4WorldPosition;
};
