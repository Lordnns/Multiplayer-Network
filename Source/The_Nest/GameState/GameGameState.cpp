// Fill out your copyright notice in the Description page of Project Settings.


#include "GameGameState.h"

#include "net/UnrealNetwork.h"

void AGameGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGameGameState, TeamScores);
	DOREPLIFETIME(AGameGameState, TimeRemaining);
	DOREPLIFETIME(AGameGameState, ScoreToWin);
	
}

void AGameGameState::SetTeamScore(int32 TeamID, int32 NewScore)
{
	if(TeamScores.IsValidIndex(TeamID))
	{
		TeamScores[TeamID] = NewScore;

		OnScoreChangedDelegate.Broadcast();
	}
}

int32 AGameGameState::GetTeamScore(int32 TeamID) const
{
	return TeamScores.IsValidIndex(TeamID) ? TeamScores[TeamID] : 0;
}

void AGameGameState::CreateTeams(int NumberOfTeams)
{
	TeamScores.Empty();
	TeamScores.SetNum(NumberOfTeams);

	for(int i =0; i<NumberOfTeams; i++ )
	{
		TeamScores[i]= 0;
	}

	OnScoreChangedDelegate.Broadcast();
}

AGameGameState::AGameGameState()
{
	TimeRemaining = 0.0f;
	TeamPlayerCounts.SetNum(2);
	TeamPlayerCounts[0] = 0;
	TeamPlayerCounts[1] = 0;
	ScoreToWin = 0;
	TeamScores.SetNum(2);
	TeamScores[0] = 0;
	TeamScores[1] = 0;
}

void AGameGameState::OnRep_TeamScores()
{

	for (int32 TeamID = 0; TeamID < TeamScores.Num(); TeamID++)
	{
		OnScoreChangedDelegate.Broadcast();
	}
}

TArray<int32> AGameGameState::GetAllTeamScores() const
{
	return TeamScores;
}

void AGameGameState::AddPlayerToTeam(int32 TeamID)
{
	if (TeamPlayerCounts.IsValidIndex(TeamID))
	{
		TeamPlayerCounts[TeamID]++;
	}
}

void AGameGameState::RemovePlayerFromTeam(int32 TeamID)
{
	if (TeamPlayerCounts.IsValidIndex(TeamID) && TeamPlayerCounts[TeamID] > 0)
	{
		TeamPlayerCounts[TeamID]--;
	}
}

int32 AGameGameState::GetPlayerCountInTeam(int32 TeamID) const
{
	return TeamPlayerCounts.IsValidIndex(TeamID) ? TeamPlayerCounts[TeamID] : 0;
}

void AGameGameState::AddSkinnToSlectedSkins(FGameplayTag SkinTag)
{
	if (!SelectedSkins.Contains(SkinTag))
		SelectedSkins.Add(SkinTag);
}

void AGameGameState::RemoveSkinFromSlectedSkins(FGameplayTag SkinTag)
{
	if (SelectedSkins.Contains(SkinTag))
		SelectedSkins.Remove(SkinTag);
}

TArray<FGameplayTag> AGameGameState::GetSelectedSkins()
{
	return SelectedSkins;
}