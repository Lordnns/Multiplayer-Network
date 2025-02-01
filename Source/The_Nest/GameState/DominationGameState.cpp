// Fill out your copyright notice in the Description page of Project Settings.


#include "DominationGameState.h"

#include "Kismet/GameplayStatics.h"
#include "The_Nest/GameMode/DominationGameMode.h"

void ADominationGameState::AddFlagZone(AFlagZone* Zone)
{
	FlagZones.Add(Zone);
}

