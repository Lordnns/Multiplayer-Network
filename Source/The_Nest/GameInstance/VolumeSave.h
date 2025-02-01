// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "VolumeSave.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API UVolumeSave : public USaveGame
{
	GENERATED_BODY()

public:
	
	UVolumeSave();
	
	// Volume settings
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Audio")
	float MasterVolume;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Audio")
	float MusicVolume;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Audio")
	float SFXVolume;

};
