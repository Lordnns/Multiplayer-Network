// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameGameMode.h"
#include "The_Nest/Actors/FlagZone.h"
#include "DominationGameMode.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API ADominationGameMode : public AGameGameMode
{
	GENERATED_BODY()
protected:
	
	void SpawnFlags();
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	float AwardPointsFrequency;

	UPROPERTY(EditAnywhere)
	int PointPerZoneCaptured;

	void AwardPoints();

	virtual void CheckWinCondition() override;

	virtual void HandlePlayerKilled(AGamePlayerState* KillerPS, AGamePlayerState* VictimPS) override;

public:
	ADominationGameMode();
	
	void HandleFlagUpdate();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Domination")
	TSubclassOf<AFlagZone> FlagZoneClass;

private:
	
	FTimerHandle AwardPointsTimerHandle;
};
