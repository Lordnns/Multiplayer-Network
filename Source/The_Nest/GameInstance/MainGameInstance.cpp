// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UMainGameInstance::SaveVolumeSettings()
{
	// Create or load the save game object
	UVolumeSave* SaveGameInstance = Cast<UVolumeSave>(UGameplayStatics::CreateSaveGameObject(UVolumeSave::StaticClass()));

	if (SaveGameInstance)
	{
		// Assign current values to the save game object
		SaveGameInstance->MasterVolume = MasterVolume;
		SaveGameInstance->MusicVolume = MusicVolume;
		SaveGameInstance->SFXVolume = SFXVolume;

		// Save to the specified slot
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, UserIndex);
	}
}

void UMainGameInstance::LoadVolumeSettings()
{
	// Check if a save game exists in the slot
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
	{
		// Load the save game object
		UVolumeSave* LoadedGame = Cast<UVolumeSave>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));

		if (LoadedGame)
		{
			// Assign loaded values to the GameInstance
			MasterVolume = LoadedGame->MasterVolume;
			MusicVolume = LoadedGame->MusicVolume;
			SFXVolume = LoadedGame->SFXVolume;
		}
	}
	else
	{
		// No save game found; set default values
		MasterVolume = 0.5f;
		MusicVolume = 0.2f;
		SFXVolume = 0.8f;
	}
}

void UMainGameInstance::Init()
{
	Super::Init();
	LoadVolumeSettings();
	
	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UMainGameInstance::HandleNetworkFailure);
	}
}

void UMainGameInstance::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	UE_LOG(LogTemp, Warning, TEXT("Network Failure: %s"), *ErrorString);

	// Check the type of failure
	if (FailureType == ENetworkFailure::ConnectionLost || FailureType == ENetworkFailure::FailureReceived)
	{
		if (World)
		{
			APlayerController* PlayerController = World->GetFirstPlayerController();
			if (PlayerController)
			{
				const FString TravelURL = FPackageName::ObjectPathToPackageName(CrashedWorldPath.ToString());
				UE_LOG(LogTemp, Log, TEXT("Traveling to Main Menu: %s"), *TravelURL);

				// Initiate client travel
				PlayerController->ClientTravel(TravelURL, TRAVEL_Absolute);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("PlayerController not found! Cannot travel to Main Menu."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("World context is null! Cannot handle network failure."));
		}
	}
}