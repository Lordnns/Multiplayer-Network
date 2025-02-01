// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameGameState.h"
#include "The_Nest/Actors/FlagZone.h"
#include "DominationGameState.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API ADominationGameState : public AGameGameState
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Domination")
	TArray<AFlagZone*> FlagZones;
	
public:
	
	void AddFlagZone(AFlagZone* Zone);

	TArray<AFlagZone*> GetFlagZone() {return FlagZones;}
	
	
};
