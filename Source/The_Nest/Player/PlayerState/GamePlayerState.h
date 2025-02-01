// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerState.h"
#include "GameplayTagContainer.h"
#include "GamePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API AGamePlayerState : public ABasePlayerState
{
	GENERATED_BODY()

public:
	AGamePlayerState();

	UFUNCTION()
	virtual void OnRep_SkinTagChanged();

	virtual void CopyProperties(APlayerState* PlayerState) override;

	UFUNCTION()
	void SetSelectedSkin(FGameplayTag SelectedSkin);

	UFUNCTION()
	FGameplayTag GetSelectedSkin() const {return SkinTag;}

	UFUNCTION()
	void SetTeamID(int id);
	UFUNCTION()
	void AddKill();
	UFUNCTION()
	void RemoveKill();
	UFUNCTION()
	void AddDeath();
	UFUNCTION()
	void RemoveDeath();
	UFUNCTION()
	void AddScore(int Modifier);
	UFUNCTION()
	void RemoveScore(int Modifier);

	UFUNCTION()
	int GetTeamID() const;

	UFUNCTION()
	void SetDeadState(bool State);

	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	bool GetDeadState() const;

	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	int32 GetPlayerScore() const;
	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	int32 GetPlayerKills() const;
	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	int32 GetPlayerDeaths() const;
	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	void SetKillIsRelevant(bool NewRelevantState);
	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	bool GetKillIsRelevfant() {return bKillIsRelevant;}

protected:

	UPROPERTY()
	bool bKillIsRelevant;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Info", ReplicatedUsing = OnRep_SkinTagChanged)
	FGameplayTag SkinTag;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Player Infos")
	int32 TeamID;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Player Infos")
	int32 Kills;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Player Infos")
	int32 Deaths;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Player Infos")
	int32 PlayerScore;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsDead;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
