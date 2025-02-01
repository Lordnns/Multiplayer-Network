// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameGameState.h"
#include "The_Nest/Actors/HardpointZone.h"
#include "HardPointGameState.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API AHardPointGameState : public AGameGameState
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HardPoint")
	TArray<AHardpointZone*> HardPointZones;
	
public:
	
	void AddFlagZone(AHardpointZone* Zone);

	TArray<AHardpointZone*> GetFlagZone() {return HardPointZones;}
	
	
	
};
