// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameState.h"
#include "GameplayTagContainer.h"
#include "LobbyGameState.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemainingTimeChanged, float, time);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyTimerStopped);

USTRUCT()
struct FVoteCount
{
	GENERATED_BODY()

	UPROPERTY()
	int32 TeamDeathmatchVotes = 0;;
	
	UPROPERTY()
	int32 DominationVotes = 0;;
	
	UPROPERTY()
	int32 HardpointVotes = 0;
 
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FVoteCount> : public TStructOpsTypeTraitsBase2<FVoteCount>
{
	enum
	{
		WithNetSerializer = true
	};
};

UCLASS()
class THE_NEST_API ALobbyGameState : public ABaseGameState
{
	GENERATED_BODY()

	ALobbyGameState();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

	UPROPERTY(ReplicatedUsing = OnRep_OnLobbyEndTimeChanged)
	float LobbyEndTime;

	UPROPERTY(BlueprintAssignable)
	FOnRemainingTimeChanged FOnRemainingTimeChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnLobbyTimerStopped FOnLobbyTimerStoppedDelegate;
	
	UFUNCTION()
	void LobbyEndTimeChanged();
	
	UFUNCTION()
	void StopLobbyTimer();
	

	UFUNCTION(BlueprintCallable, Category = "Game Info")
	void GetCurrentVoteCounts(int32& OutTeamDeathmatchVotes, int32& OutDominationVotes, int32& OutHardpointVotes) const;

	void SetCurrentVoteCounts();

	UPROPERTY(ReplicatedUsing = OnRep_VoteCounts)
	FVoteCount VoteCounts;

	void AddSkinnToSlectedSkins(FGameplayTag SkinTag);

	void RemoveSkinFromSlectedSkins(FGameplayTag SkinTag);

	TArray<FGameplayTag> GetSelectedSkins();

	

	
protected:

	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<FGameplayTag> SelectedSkins;
	
	UFUNCTION()
	void OnRep_OnLobbyEndTimeChanged();

	UFUNCTION()
	void OnRep_VoteCounts();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StopLobbyTimer();
	
	
	
};
