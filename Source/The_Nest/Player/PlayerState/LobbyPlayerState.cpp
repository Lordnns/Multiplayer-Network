// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerState.h"

#include "GamePlayerState.h"
#include "The_Nest/DevelopperSettings/HeroSettings.h"
#include "The_Nest/Player/Character/ThirdPersonCharacter.h"
#include "The_Nest/Player/PlayerController/LobbyPlayerController.h"

void ALobbyPlayerState::OnRep_SkinTagChanged()
{
	if (AThirdPersonCharacter* Pawn = Cast<AThirdPersonCharacter>(GetPawn()))
	{
		Pawn->ApplySkinOnCharacter();
	}
}

void ALobbyPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if (AGamePlayerState* NewPlayerState = Cast<AGamePlayerState>(PlayerState))
	{
		NewPlayerState->SetSelectedSkin(SkinTag);
	}
	
}

void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyPlayerState, SkinTag);
}

ALobbyPlayerState::ALobbyPlayerState()
{
	VoteChoice = 4;
}

void ALobbyPlayerState::SetSelectedSkin(FGameplayTag SelectedSkin)
{
	SkinTag = SelectedSkin;
}
