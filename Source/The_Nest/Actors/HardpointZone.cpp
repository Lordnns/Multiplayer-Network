// Fill out your copyright notice in the Description page of Project Settings.


#include "HardpointZone.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "The_Nest/Player/Character/ThirdPersonCharacter.h"
#include "The_Nest/Player/PlayerState/GamePlayerState.h"

void AHardpointZone::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AHardpointZone, ControllingTeamID);
}

void AHardpointZone::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority()) return;
	
	if (OtherActor && OtherActor->IsA(ACharacter::StaticClass()))
	{
		if(AGamePlayerState* PS = Cast<AGamePlayerState>(Cast<AThirdPersonCharacter>(OtherActor)))
			PS->SetKillIsRelevant(false);
		PlayersInZone.Remove(OtherActor);
		UpdatePlayerCountInTeam();
	}
}

void AHardpointZone::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	
	if (OtherActor && OtherActor->IsA(ACharacter::StaticClass()))
	{
		if(AGamePlayerState* PS = Cast<AGamePlayerState>(Cast<AThirdPersonCharacter>(OtherActor)))
			PS->SetKillIsRelevant(true);
		PlayersInZone.Add(OtherActor);
		UpdatePlayerCountInTeam();
	}
}

void AHardpointZone::UpdatePlayerCountInTeam()
{
	int TempA = 0;
	int TempB = 0;
	
	for (AActor* Player : PlayersInZone)
	{
		if (ACharacter* Character = Cast<ACharacter>(Player))
		{
			if (AGamePlayerState* PlayerState = Cast<AGamePlayerState>(Character->GetPlayerState()))
			{
				int32 PlayerTeamID = PlayerState->GetTeamID();
				if (PlayerTeamID == 0)
				{
					TempA++;
				}
				else if (PlayerTeamID == 1)
				{
					TempB++;
				}
			}
		}
	}
	TeamACount = TempA;
	TeamBCount = TempB;
	
	if (TeamACount > 0 && TeamBCount > 0)
	{
		// Contested: No progress
		UE_LOG(LogTemp, Log, TEXT("Flag %s is contested."), *GetName());
		StopScoring();
	}
	else if (TeamACount > 0)
	{
		StartScoring(0);
	}
	else if (TeamBCount > 0)
	{
		StartScoring(1);
	}
	else
	{
		StopScoring();
	}

}
