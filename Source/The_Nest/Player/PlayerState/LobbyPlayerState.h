// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerState.h"
#include "GameplayTagContainer.h"
#include "LobbyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API ALobbyPlayerState : public ABasePlayerState
{
	GENERATED_BODY()

public:
	ALobbyPlayerState();

	UFUNCTION()
	void SetSelectedSkin(FGameplayTag SelectedSkin);

	UFUNCTION()
	FGameplayTag GetSelectedSkin() const {return SkinTag;}

	UFUNCTION()
	virtual void OnRep_SkinTagChanged();

	virtual void CopyProperties(APlayerState* PlayerState) override;
	

	UPROPERTY()
	int VoteChoice;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Info", ReplicatedUsing = OnRep_SkinTagChanged)
	FGameplayTag SkinTag;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
		
	
};
