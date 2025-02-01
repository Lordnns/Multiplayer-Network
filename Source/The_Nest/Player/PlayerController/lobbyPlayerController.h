// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerController.h"
#include "GameplayTagContainer.h"
#include "The_Nest/GameState/LobbyGameState.h"
#include "The_Nest/Player/Character/ThirdPersonCharacter.h"
#include "lobbyPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerEndValueChanged, float, Time);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConfirmedHero);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTurnOffTimer);

/**
 * 
 */
UCLASS()
class THE_NEST_API ALobbyPlayerController : public ABasePlayerController
{
	GENERATED_BODY()

protected:

	virtual void SetupInputComponent() override;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetPlayerName(const FText& Name);

	UFUNCTION()
	void ServerSetPlayerName_Implementation(const FText& Name);
	UFUNCTION()
	bool ServerSetPlayerName_Validate(const FText& Name);

	UFUNCTION()
	void HandleLobbyTimerStopped();
	
	void BeginPlay() override;

	UFUNCTION()
	void HandleRemainingTimeChanged(float time);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerConfirmPlayerSkin(FGameplayTag Tag, class ALobbyPlayerState* RequestingPlayer, bool bIsFirstSelection);

	UFUNCTION(Client, Reliable)
	void ClientConfirmPlayerSkin(bool RequestAccepted, bool bIsFirstSelection);

	UFUNCTION()
	bool ConfirmPlayerSkin(FGameplayTag Tag, class ALobbyPlayerState* RequestingPlayer, bool bIsFirstSelection) const;

	virtual void ReplicationStarted_Implementation() override;

public:
	UPROPERTY(BlueprintAssignable)
	FOnTimerEndValueChanged OnTimerEndValueChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnTurnOffTimer OnTimerOffDelegate;
	
	UFUNCTION(BlueprintCallable, Category = "Networking")
	void ConfirmSkinSelection(FGameplayTag Tag, bool bIsFirstSelection);

	UPROPERTY(BlueprintAssignable, Category= "UI")
	FOnConfirmedHero OnConfirmedHeroDelegate;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void EnableDisableFocus();

	UFUNCTION(Server, Reliable)
	void ServerSetVote(int Choice);

	void SetVote(int Choice);

	UFUNCTION(BlueprintCallable, Category = "Game Stats")
	void ReceiveVote(int Choice);

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Stats")
	void UpdateVoteCountsUI();
};
