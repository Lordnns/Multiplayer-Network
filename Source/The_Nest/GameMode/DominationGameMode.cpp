// Fill out your copyright notice in the Description page of Project Settings.


#include "DominationGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "The_Nest/Actors/FlagPlaceholder.h"
#include "The_Nest/Actors/FlagZone.h"
#include "The_Nest/GameState/DominationGameState.h"
#include "The_Nest/Player/PlayerState/GamePlayerState.h"


void ADominationGameMode::SpawnFlags()
{
	if(!HasAuthority()) return;
	
	// Get all flag placeholders in the level
	TArray<AActor*> FlagPlaceholders;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFlagPlaceholder::StaticClass(), FlagPlaceholders);

	for (AActor* Placeholder : FlagPlaceholders)
	{
		if (AFlagPlaceholder* FlagPlaceholder = Cast<AFlagPlaceholder>(Placeholder))
		{
			// Spawn the flag zone at the placeholder's location
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;

			

			AFlagZone* FlagZone = GetWorld()->SpawnActor<AFlagZone>(
				FlagZoneClass,
				FlagPlaceholder->GetActorLocation(),
				FlagPlaceholder->GetActorRotation(),
				SpawnParams
			);

			if (FlagZone)
			{
				// Assign the flag's name from the placeholder
				FlagZone->SetFlagName(FlagPlaceholder->FlagName);

				// Add the flag zone to the GameState for tracking
				if (ADominationGameState* GM = GetGameState<ADominationGameState>())
				{
					GM->AddFlagZone(FlagZone);
					FlagZone->SetOwner(this);
				}

				UE_LOG(LogTemp, Log, TEXT("Spawned Flag Zone %s at location %s"), 
					*FlagPlaceholder->FlagName.ToString(), 
					*FlagPlaceholder->GetActorLocation().ToString());
			}
		}
	}
}

void ADominationGameMode::BeginPlay()
{
	Super::BeginPlay();

	SpawnFlags();
	if(ADominationGameState* GS = GetGameState<ADominationGameState>())
	{
		GS->SetScoreToWin(ScoreToWin);
	}

	GetWorldTimerManager().SetTimer(AwardPointsTimerHandle, this, &ADominationGameMode::AwardPoints, AwardPointsFrequency, true);
}

ADominationGameMode::ADominationGameMode()
{
	AwardPointsFrequency = 1;
	PointPerZoneCaptured = 1;
}

void ADominationGameMode::HandleFlagUpdate()
{
	CheckWinCondition();
}

void ADominationGameMode::AwardPoints()
{
	if(ADominationGameState* GS = GetGameState<ADominationGameState>())
	{
		TArray<AFlagZone*> Zones = GS->GetFlagZone();
		for (AFlagZone* Zone : Zones)
		{
			if (Zone && Zone->GetControllingTeamID() != -1)
			{
				const int32 NewScore = GS->GetTeamScore(Zone->GetControllingTeamID()) + PointPerZoneCaptured;
				GS->SetTeamScore(Zone->GetControllingTeamID(), NewScore);
			}
		}

		HandleFlagUpdate();
	}
}

void ADominationGameMode::CheckWinCondition()
{
	ADominationGameState* GS = GetGameState<ADominationGameState>();
	if (!GS) return;

	for (int32 i = 0; i < TeamCount; i++)
	{
		if (GS->GetTeamScore(i) >= ScoreToWin)
		{
			// end game here "i" team is the winner
			GetWorldTimerManager().ClearTimer(AwardPointsTimerHandle);
			HandleTeamWin(i);
			break;
		}
	}
}

void ADominationGameMode::HandlePlayerKilled(AGamePlayerState* KillerPS, AGamePlayerState* VictimPS)
{
	Super::HandlePlayerKilled(KillerPS, VictimPS);
	
	if (!KillerPS || !VictimPS) return;

	AGameGameState* GS = GetGameState<AGameGameState>();
	if (GS)
	{
		// Check if victim and killer are on the same team
		if (!KillerPS->GetTeamID() == VictimPS->GetTeamID())
		{
			if(VictimPS->GetKillIsRelevfant())
			{
				KillerPS->AddScore(PlayerPointPerRelevantKill);
				//maybe trigger ui if we have time
			}
			else
			{
				KillerPS->AddScore(PlayerPointPerKill);
				// same here
			}
		}
	}
}
