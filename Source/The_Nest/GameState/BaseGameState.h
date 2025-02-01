// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BaseGameState.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API ABaseGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ABaseGameState() { bReplicates = true; }

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayerLeftMessage(const FString& PlayerName);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayerJoinMessage(const FString& PlayerName);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_TravelToMap(const FString& MapPath);
	
};