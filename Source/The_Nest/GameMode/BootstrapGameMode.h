// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameMode.h"
#include "BootstrapGameMode.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API ABootstrapGameMode : public ABaseGameMode
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable, Category = "Travel")
	void OpenMainMenuLevel();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> MainMenuWorldPath;
	
};
