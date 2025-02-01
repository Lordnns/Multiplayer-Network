// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameState.h"

void ABaseGameState::Multicast_PlayerLeftMessage_Implementation(const FString& PlayerName)
{
	UE_LOG(LogTemp, Log, TEXT("Broadcasting to clients: Player %s has left the game."), *PlayerName);

	// You can also use this to trigger UI or HUD events in the client
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player %s has left the game."), *PlayerName));
}

void ABaseGameState::Multicast_TravelToMap_Implementation(const FString& MapPath)
{
	if (HasAuthority())
		return; // Skip ClientTravel for the host
	
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		UE_LOG(LogTemp, Log, TEXT("Multicast: Traveling to map %s"), *MapPath);
		PlayerController->ClientTravel(MapPath, TRAVEL_Absolute);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MulticastTravelToMap: No PlayerController found."));
	}
}

void ABaseGameState::Multicast_PlayerJoinMessage_Implementation(const FString& PlayerName)
{
	UE_LOG(LogTemp, Log, TEXT("Broadcasting to clients: Player %s has joined the game."), *PlayerName);

	// You can also use this to trigger UI or HUD events in the client
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player %s has joined the game."), *PlayerName));
}
