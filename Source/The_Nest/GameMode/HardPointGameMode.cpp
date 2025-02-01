// Fill out your copyright notice in the Description page of Project Settings.


#include "HardPointGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "The_Nest/Actors/FlagPlaceholder.h"
#include "The_Nest/GameState/HardPointGameState.h"
#include "The_Nest/Player/PlayerState/GamePlayerState.h"

void AHardPointGameMode::SpawnFlags()
{
	if (FlagPlaceholders.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No flag placeholders available!"));
        return;
    }

    // Get the current placeholder
    AFlagPlaceholder* CurrentPlaceholder = Cast<AFlagPlaceholder>(FlagPlaceholders[CurrentPlaceholderIndex]);
    if (!CurrentPlaceholder)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid placeholder at index %d"), CurrentPlaceholderIndex);
        return;
    }

    // Destroy the previous zone if it exists
    if (ActiveZone && ActiveZone->IsValidLowLevel())
    {
    	ActiveZone->Destroy();
    }

    // Spawn the new hardpoint zone at the current placeholder's location
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;

    AHardpointZone* NewHardPointZone = GetWorld()->SpawnActor<AHardpointZone>(
        HardPointZoneClass,
        CurrentPlaceholder->GetActorLocation(),
        CurrentPlaceholder->GetActorRotation(),
        SpawnParams
    );

    if (NewHardPointZone)
    {
        // Assign the flag's name from the placeholder
        NewHardPointZone->SetFlagName(CurrentPlaceholder->FlagName);
        ActiveZone = NewHardPointZone; // Store the active zone

        // Add the flag zone to the GameState for tracking
        if (AHardPointGameState* GS = GetGameState<AHardPointGameState>())
        {
            GS->AddFlagZone(NewHardPointZone);
            NewHardPointZone->SetOwner(this);
        }

        UE_LOG(LogTemp, Log, TEXT("Spawned Zone %s at location %s"),
            *CurrentPlaceholder->FlagName.ToString(),
            *CurrentPlaceholder->GetActorLocation().ToString());
    }

    // Move to the next placeholder index, loop back if necessary
    CurrentPlaceholderIndex = (CurrentPlaceholderIndex + 1) % FlagPlaceholders.Num();
}

void AHardPointGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFlagPlaceholder::StaticClass(), FlagPlaceholders);

	CurrentPlaceholderIndex =0;
	
	SpawnFlags();
	
	if(AHardPointGameState* GS = GetGameState<AHardPointGameState>())
	{
		GS->SetScoreToWin(ScoreToWin);
	}

	GetWorldTimerManager().SetTimer(RotateFlagTimerHandle, this, &AHardPointGameMode::SpawnFlags, RotationFrequency, true);
	GetWorldTimerManager().SetTimer(AwardPointsTimerHandle, this, &AHardPointGameMode::AwardPoints, AwardPointsFrequency, true);
}

void AHardPointGameMode::AwardPoints()
{
	if(AHardPointGameState* GS = GetGameState<AHardPointGameState>())
	{
		TArray<AHardpointZone*> Zones = GS->GetFlagZone();
		for (AHardpointZone* Zone : Zones)
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

void AHardPointGameMode::CheckWinCondition()
{
	AHardPointGameState* GS = GetGameState<AHardPointGameState>();
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

void AHardPointGameMode::HandlePlayerKilled(AGamePlayerState* KillerPS, AGamePlayerState* VictimPS)
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

AHardPointGameMode::AHardPointGameMode()
{
	AwardPointsFrequency = 1;
	PointPerZoneCaptured = 1;
}

void AHardPointGameMode::HandleFlagUpdate()
{
	CheckWinCondition();
}
