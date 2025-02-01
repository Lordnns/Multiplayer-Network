// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlayerState.h"

#include "The_Nest/Player/Character/ThirdPersonCharacter.h"
#include "The_Nest/Player/PlayerController/GamePlayerController.h"

AGamePlayerState::AGamePlayerState()
{
	PlayerScore = 0;
	Kills = 0;
	Deaths = 0;
	bIsDead = false;
	TeamID = 0;
}

void AGamePlayerState::OnRep_SkinTagChanged()
{
	if (AThirdPersonCharacter* Pawn = Cast<AThirdPersonCharacter>(GetPawn()))
	{
		Pawn->ApplySkinOnCharacter();
	}
}

void AGamePlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	
}

void AGamePlayerState::SetSelectedSkin(FGameplayTag SelectedSkin)
{
	SkinTag = SelectedSkin;
}

void AGamePlayerState::SetTeamID(int id)
{
	TeamID = id;
}

void AGamePlayerState::AddKill()
{
	Kills++;
}

void AGamePlayerState::RemoveKill()
{
	Kills--;

	if(Kills < 0)
	{
		Kills = 0;
	}
}

void AGamePlayerState::AddDeath()
{
	Deaths++;
}

void AGamePlayerState::RemoveDeath()
{
	Deaths--;
	if(Deaths < 0)
	{
		Deaths = 0;
	}
}

void AGamePlayerState::AddScore(int Modifier)
{
	PlayerScore += Modifier;
}

void AGamePlayerState::RemoveScore(int Modifier)
{
	PlayerScore -= Modifier;
}

int AGamePlayerState::GetTeamID() const
{
	return TeamID;
}

void AGamePlayerState::SetDeadState(bool State)
{
	bIsDead = State;
}

bool AGamePlayerState::GetDeadState() const
{
	return bIsDead;
}

int32 AGamePlayerState::GetPlayerScore() const
{
	return PlayerScore;
}

int32 AGamePlayerState::GetPlayerKills() const
{
	return Kills;
}

int32 AGamePlayerState::GetPlayerDeaths() const
{
	return Deaths;
}

void AGamePlayerState::SetKillIsRelevant(bool NewRelevantState)
{
	bKillIsRelevant = NewRelevantState;
}

void AGamePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGamePlayerState, TeamID);
	DOREPLIFETIME(AGamePlayerState, Kills);
	DOREPLIFETIME(AGamePlayerState, Deaths);
	DOREPLIFETIME(AGamePlayerState, bIsDead);
	DOREPLIFETIME(AGamePlayerState, SkinTag);
	DOREPLIFETIME(AGamePlayerState, PlayerScore);
}
