// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerState.h"

ABasePlayerState::ABasePlayerState()
{
	AvgLatency = 0;
	bIsHost = true;
	PlayerName = FText::FromString("");
	bReplicates = true;
}

void ABasePlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	if (ABasePlayerState* NewPlayerState = Cast<ABasePlayerState>(PlayerState))
	{
		NewPlayerState->PlayerName = PlayerName;
	}
}

void ABasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Ensure that AvgLatency is replicated to all clients
	DOREPLIFETIME(ABasePlayerState, AvgLatency);
	DOREPLIFETIME(ABasePlayerState, bIsHost);
	DOREPLIFETIME(ABasePlayerState, PlayerName);
}
