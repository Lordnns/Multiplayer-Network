// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameGameMode.h"
#include "TDMGameMode.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API ATDMGameMode : public AGameGameMode
{
	GENERATED_BODY()

public:
	ATDMGameMode();
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int ScoreByKill;

	virtual void BeginPlay() override;
	
	virtual void HandlePlayerKilled(AGamePlayerState* KillerPS, AGamePlayerState* VictimPS) override;

	virtual void CheckWinCondition() override;
};
