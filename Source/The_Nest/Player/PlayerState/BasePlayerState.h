// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerState.h"
#include "net/UnrealNetwork.h"
#include "BasePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API ABasePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ABasePlayerState();
	
	virtual void CopyProperties(APlayerState* PlayerState) override;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Networking")
	int32 AvgLatency;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Networking")
	bool bIsHost;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player Info")
	FText PlayerName;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
