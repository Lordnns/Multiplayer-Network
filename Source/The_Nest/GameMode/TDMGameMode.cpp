// Fill out your copyright notice in the Description page of Project Settings.


#include "TDMGameMode.h"

#include "The_Nest/GameState/TDMGameState.h"
#include "The_Nest/Player/PlayerState/GamePlayerState.h"


ATDMGameMode::ATDMGameMode()
{

}

void ATDMGameMode::BeginPlay()
{
	Super::BeginPlay();

	if(ATDMGameState* GS = GetGameState<ATDMGameState>())
	{
		GS->SetScoreToWin(ScoreToWin);
	}
}

void ATDMGameMode::HandlePlayerKilled(AGamePlayerState* KillerPS, AGamePlayerState* VictimPS)
{
	Super::HandlePlayerKilled(KillerPS, VictimPS);
	
	if (!KillerPS || !VictimPS) return;

	ATDMGameState* GS = GetGameState<ATDMGameState>();
	if (GS)
	{
		// Check if victim and killer are on the same team
		if (KillerPS->GetTeamID() == VictimPS->GetTeamID())
		{
			KillerPS->RemoveScore(PlayerPointPerKill);
			int32 CurrentScore = GS->GetTeamScore(KillerPS->GetTeamID());
			GS->SetTeamScore(KillerPS->GetTeamID(), CurrentScore - ScoreByKill);
			
		}
		else
		{
			KillerPS->AddScore(PlayerPointPerKill);
			int32 NewScore = GS->GetTeamScore(KillerPS->GetTeamID()) + ScoreByKill;
			GS->SetTeamScore(KillerPS->GetTeamID(), NewScore);
		}
		CheckWinCondition();
	}
}

void ATDMGameMode::CheckWinCondition()
{
	ATDMGameState* GS = GetGameState<ATDMGameState>();
	if (!GS) return;

	for (int32 i = 0; i < TeamCount; i++)
	{
		if (GS->GetTeamScore(i) >= ScoreToWin)
		{
			// end game here "i" team is the winner
			HandleTeamWin(i);
			break;
		}
	}
}
