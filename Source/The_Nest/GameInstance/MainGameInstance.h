// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "VolumeSave.h"
#include "MainGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, Category = "Sounds Controls")
	float MasterVolume;

	UPROPERTY(BlueprintReadWrite, Category = "Sounds Controls")
	float MusicVolume;

	UPROPERTY(BlueprintReadWrite, Category = "Sounds Controls")
	float SFXVolume;
	
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	FText PlayerName;

	// Save and Load Functions
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void SaveVolumeSettings();

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void LoadVolumeSettings();
	virtual void Init() override;
	
protected:

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> CrashedWorldPath;
	
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	

private:
	FString SaveSlotName = TEXT("VolumeSettings");
	uint32 UserIndex = 0;
	
};
