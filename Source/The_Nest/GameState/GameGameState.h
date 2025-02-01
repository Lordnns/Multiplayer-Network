// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameState.h"
#include "GameplayTagContainer.h"
#include "GameGameState.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API AGameGameState : public ABaseGameState
{
	GENERATED_BODY()

public:
	AGameGameState();

protected:

	UPROPERTY(BlueprintReadOnly)
	TArray<FGameplayTag> SelectedSkins;
	
	UPROPERTY(ReplicatedUsing = OnRep_TeamScores, BlueprintReadOnly, Category = "Game Infos")
	TArray<int32> TeamScores;


	UFUNCTION()
	void OnRep_TeamScores();

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Game Infos")
	float TimeRemaining;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(BlueprintReadOnly, Category = "Teams")
	TArray<int32> TeamPlayerCounts;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Replicated)
	int ScoreToWin;


public:

	UFUNCTION(BlueprintCallable, Category = "Game Rules")
	int GetScoreToWin() {return ScoreToWin;}

	void SetScoreToWin(int NewScore){ScoreToWin = NewScore;}

	void AddSkinnToSlectedSkins(FGameplayTag SkinTag);

	void RemoveSkinFromSlectedSkins(FGameplayTag SkinTag);

	TArray<FGameplayTag> GetSelectedSkins();
	
	UFUNCTION(BlueprintCallable, Category = "Teams")
	void AddPlayerToTeam(int32 TeamID);

	UFUNCTION(BlueprintCallable, Category = "Teams")
	void RemovePlayerFromTeam(int32 TeamID);

	UFUNCTION(BlueprintCallable, Category = "Teams")
	int32 GetPlayerCountInTeam(int32 TeamID) const;
	
	UFUNCTION()
	void SetTeamScore(int32 TeamID, int32 NewScore);
	
	UFUNCTION()
	int32 GetTeamScore(int32 TeamID) const;

	UFUNCTION()
	void CreateTeams(int NumberOfTeams);
	
	UFUNCTION(BlueprintCallable)
	TArray<int32> GetAllTeamScores() const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScoreChanged);
	
	UPROPERTY(BlueprintAssignable)
	FOnScoreChanged OnScoreChangedDelegate;
	
};
