// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"

#include "OnlineSubsystem.h"
#include "The_Nest/GameInstance/MainGameInstance.h"

void AMainMenuGameMode::Host(FText& PlayerName)
{
	if (!LobbyWorldPath.IsNull())
	{
		// Directly use the object path for travel
		const FString LevelPath = FPackageName::ObjectPathToPackageName(LobbyWorldPath.ToString());
		const FString TravelURL = FString::Printf(TEXT("%s?listen?PlayerName=%s?bIsLanMatch=1&game=Lobby"), *LevelPath, *PlayerName.ToString());
		GetWorld()->ServerTravel(TravelURL);
	}
}

void AMainMenuGameMode::Join(const FString& ServerAddress, FText& PlayerName)
{
	if (APlayerController* PlayerController = GetWorld()->GetGameInstance()->GetFirstLocalPlayerController())
	{
		const FString TravelURL = FString::Printf(TEXT("%s?PlayerName=%s?bIsLanMatch=1"), *ServerAddress, *PlayerName.ToString());
        
		PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
	}
}