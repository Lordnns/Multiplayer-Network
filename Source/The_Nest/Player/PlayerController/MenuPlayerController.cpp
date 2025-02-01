// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuPlayerController.h"

void AMenuPlayerController::ClientTravelToMainMenu()
{
	if (MainMenuWorldPath.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("ClientTravelToMap: MapPath is empty. Aborting travel."));
		return;
	}

	const FString LevelPath = FPackageName::ObjectPathToPackageName(MainMenuWorldPath.ToString());
	UE_LOG(LogTemp, Log, TEXT("ClientTravel to map: %s"), *LevelPath);

	// Call the built-in ClientTravel function
	ClientTravel(LevelPath, TRAVEL_Absolute);
}