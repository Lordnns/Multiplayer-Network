// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameState.h"

#include "Net/UnrealNetwork.h"
#include "The_Nest/Player/PlayerController/lobbyPlayerController.h"
#include "The_Nest/Player/PlayerState/LobbyPlayerState.h"

ALobbyGameState::ALobbyGameState()
{
	VoteCounts.TeamDeathmatchVotes = 0;
	VoteCounts.DominationVotes = 0;
	VoteCounts.HardpointVotes = 0;
	
}

void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyGameState, LobbyEndTime);
	DOREPLIFETIME(ALobbyGameState, SelectedSkins);
	DOREPLIFETIME(ALobbyGameState, VoteCounts);
}


void ALobbyGameState::OnRep_OnLobbyEndTimeChanged()
{
	FOnRemainingTimeChangedDelegate.Broadcast(LobbyEndTime);
}

void ALobbyGameState::OnRep_VoteCounts()
{
	if(ALobbyPlayerController* LPC = Cast<ALobbyPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		LPC->UpdateVoteCountsUI();
	}
}

void ALobbyGameState::Multicast_StopLobbyTimer_Implementation()
{
	FOnLobbyTimerStoppedDelegate.Broadcast();
}

void ALobbyGameState::LobbyEndTimeChanged()
{
	FOnRemainingTimeChangedDelegate.Broadcast(LobbyEndTime);
}

void ALobbyGameState::StopLobbyTimer()
{
	Multicast_StopLobbyTimer();
}

void ALobbyGameState::GetCurrentVoteCounts(int32& OutTeamDeathmatchVotes, int32& OutDominationVotes, int32& OutHardpointVotes) const
{
	OutTeamDeathmatchVotes = VoteCounts.TeamDeathmatchVotes;
	OutDominationVotes = VoteCounts.DominationVotes;
	OutHardpointVotes = VoteCounts.HardpointVotes;
}

void ALobbyGameState::SetCurrentVoteCounts()
{
	if (!HasAuthority())
	{
		return; // Ensure this function is only executed on the server
	}

	// Temporary variables to tally votes
	int32 TempTeamDeathmatchVotes = 0;
	int32 TempDominationVotes = 0;
	int32 TempHardpointVotes = 0;

	// Count votes from each player
	for (APlayerState* PS : PlayerArray)
	{
		if (ALobbyPlayerState* LobbyPS = Cast<ALobbyPlayerState>(PS))
		{
			switch (LobbyPS->VoteChoice)
			{
			case 0: // Team Deathmatch
				TempTeamDeathmatchVotes++;
				break;
			case 1: // Domination
				TempDominationVotes++;
				break;
			case 2: // Hardpoint
				TempHardpointVotes++;
				break;
			default:
				break;
			}
		}
	}

	VoteCounts.TeamDeathmatchVotes = TempTeamDeathmatchVotes;
	VoteCounts.DominationVotes = TempDominationVotes;
	VoteCounts.HardpointVotes = TempHardpointVotes;

	if(ALobbyPlayerController* LPC = Cast<ALobbyPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		LPC->UpdateVoteCountsUI();
	}
}

void ALobbyGameState::AddSkinnToSlectedSkins(FGameplayTag SkinTag)
{
	if (!SelectedSkins.Contains(SkinTag))
		SelectedSkins.Add(SkinTag);
}

void ALobbyGameState::RemoveSkinFromSlectedSkins(FGameplayTag SkinTag)
{
	if (SelectedSkins.Contains(SkinTag))
		SelectedSkins.Remove(SkinTag);
}

TArray<FGameplayTag> ALobbyGameState::GetSelectedSkins()
{
	return SelectedSkins;
}

bool FVoteCount::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << TeamDeathmatchVotes;
	Ar << DominationVotes;
	Ar << HardpointVotes;
	bOutSuccess = true;
	return true;
}




