// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameMode.h"
#include "MainMenuGameMode.generated.h"

UCLASS()
class THE_NEST_API AMainMenuGameMode : public ABaseGameMode
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Travel")
	TSoftObjectPtr<UWorld> LobbyWorldPath;

public:
	
	UFUNCTION(BlueprintCallable, Category = "Networking")
	void Host(FText& PlayerName);

	UFUNCTION(BlueprintCallable, Category = "Networking")
	void Join(const FString& ServerAddress, FText& PlayerName);
	
};
