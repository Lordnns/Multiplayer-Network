// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API AMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Travel")
	void ClientTravelToMainMenu();

private:
	
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> MainMenuWorldPath;
};
