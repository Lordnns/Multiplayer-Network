// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameMode.h"
#include "GameplayTagContainer.h"
#include "The_Nest/DevelopperSettings/HeroSettings.h"
#include "The_Nest/GameState/GameGameState.h"
#include "The_Nest/Player/Character/ThirdPersonCharacter.h"
#include "The_Nest/Player/PlayerController/GamePlayerController.h"
#include "GameGameMode.generated.h"

class AGamePlayerState;
/**
 * 
 */
UCLASS()
class THE_NEST_API AGameGameMode : public ABaseGameMode
{
	GENERATED_BODY()
public:
	AGameGameMode();

protected:

	// score to win the game
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int ScoreToWin;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int PlayerPointPerKill;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int PlayerPointPerRelevantKill;
	//time to spawn the player after death
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RespawnTime;

	//Maximum time allowed by game
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxGameTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int TeamCount;

	UPROPERTY(EditDefaultsOnly)
	int PlayersArrivedCount = 0;

	virtual void PostSeamlessTravel() override;

	virtual void HandleSeamlessTravelPlayer(AController*& C) override;

	UFUNCTION()
	virtual void CheckWinCondition();

	FTimerHandle GameTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float EndGameDelayTime;

	UFUNCTION()
	virtual void MaxTimeReached();

	UFUNCTION()
	virtual void HandleTeamWin(int32 WinningTeamID);

	UFUNCTION()
	virtual void HandleDraw();
	void BroadcastEndGameScreen(bool bTeamWon, int32 WinningTeamID);
	void DisableAllPlayerInputs();
	void PerformSeamlessTravel();

	UFUNCTION()
	void DistributeTeams();

	virtual void Logout(AController* Exiting) override;
	FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId,
	                      const FString& Options,
	                      const FString& Portal);

public:
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void AssignPlayerToTeam(AGameGameState* GS, AGamePlayerState* PlayerState);

	UFUNCTION()
	virtual void HandlePlayerKilled(AGamePlayerState* KillerPS, AGamePlayerState* VictimPS);

	virtual void RespawnPlayer(AGamePlayerController* PC);

	virtual void TeleportPlayer(AGamePlayerController* PC);
	
	virtual AActor* FindRandomPlayerStart(AController* Player, const FString& IncomingName);

	float GetRespawnTime() const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DisablePlayerInputs();
	
	void HandleDeath(AGamePlayerController* HitPlayerController, AThirdPersonCharacter* HitPlayer);

private:
	static FGameplayTag FindNextAvailableSkin(AGameGameState* GameState, const TArray<FHeroData>& HeroList);
	
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> LobbyWorldPath;
	
};
