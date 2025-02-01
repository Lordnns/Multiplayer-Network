// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "The_Nest/GameMode/BaseGameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API ALobbyGameMode : public ABaseGameMode
{
	GENERATED_BODY()
	
public:

	UFUNCTION()
	virtual void PostLogin(APlayerController* NewPlayer) override;

	UFUNCTION()
	void StartTravelTimer();
	
	ALobbyGameMode();

	UFUNCTION(BlueprintCallable, Category = "Validation")
	bool ValidateAndRegisterSkin(FGameplayTag& SkinTag, class ALobbyPlayerState* RequestingPlayer);


private:

	UPROPERTY()
	FGameplayTag DefaultSkinTag;

	UFUNCTION()
	void StartGame();
	
	int32 GetWinningChoice(int32 TeamDeathmatchVotes, int32 DominationVotes, int32 HardpointVotes);

	FString HandleGameModeSelection(int32 GameModeChoice);

	UFUNCTION()
	float CalculateEndOfTimer() const;

	UPROPERTY(EditAnywhere)
	float TimeBeforeGameStart;

	UPROPERTY(EditAnywhere)
	int NumberOfPlayerToStartTimer;

	UFUNCTION()
	void TravelToGameLevel();

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> GameWorldPath;

	UFUNCTION()
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;

	UPROPERTY()
	bool bTimerIsSet;

	FTimerHandle LobbyCountDown;

protected:
	
	virtual void Logout(AController* Exiting) override;

	virtual void PostSeamlessTravel() override;
	
};

