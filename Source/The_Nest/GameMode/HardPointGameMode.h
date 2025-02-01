// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameGameMode.h"
#include "The_Nest/Actors/FlagPlaceholder.h"
#include "The_Nest/Actors/HardpointZone.h"
#include "HardPointGameMode.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API AHardPointGameMode : public AGameGameMode
{
	GENERATED_BODY()

protected:

	void SpawnFlags();
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	float AwardPointsFrequency;

	UPROPERTY(EditAnywhere)
	int PointPerZoneCaptured;

	UPROPERTY(EditDefaultsOnly)
	float RotationFrequency;

	UPROPERTY()
	TArray<AActor*> FlagPlaceholders;
	
	int32 CurrentPlaceholderIndex;

	UPROPERTY()
	AHardpointZone* ActiveZone;


	void AwardPoints();

	virtual void CheckWinCondition() override;

	virtual void HandlePlayerKilled(AGamePlayerState* KillerPS, AGamePlayerState* VictimPS) override;

public:
	AHardPointGameMode();
	
	void HandleFlagUpdate();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HardPoint")
	TSubclassOf<AHardpointZone> HardPointZoneClass;

private:
	
	FTimerHandle AwardPointsTimerHandle;

	FTimerHandle RotateFlagTimerHandle;
	
};
