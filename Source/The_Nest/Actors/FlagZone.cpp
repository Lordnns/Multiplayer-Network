// Fill out your copyright notice in the Description page of Project Settings.


#include "FlagZone.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "The_Nest/GameState/DominationGameState.h"
#include "The_Nest/Player/Character/ThirdPersonCharacter.h"
#include "The_Nest/Player/PlayerState/GamePlayerState.h"


AFlagZone::AFlagZone()
{
	PrimaryActorTick.bCanEverTick = false;
	ControllingTeamID = -1;
	CaptureUpdateDeltaTime = 0.1f;
	bReplicates = true;
	bAlwaysRelevant = true;

	TeamACount = 0;
	TeamBCount = 0;

	// Create and initialize the overlap component
	OverlapComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComponent"));
	OverlapComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapComponent->SetCollisionObjectType(ECC_WorldDynamic);
	OverlapComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapComponent->SetupAttachment(RootComponent);

	OverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &AFlagZone::OnComponentBeginOverlap);
	OverlapComponent->OnComponentEndOverlap.AddDynamic(this, &AFlagZone::OnComponentEndOverlap);
}

void AFlagZone::SetFlagName(FName name)
{
	FlagName = name;
}

void AFlagZone::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFlagZone, ControllingTeamID);
	DOREPLIFETIME(AFlagZone, CaptureProgressInt);
}

void AFlagZone::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFlagZone::OnRep_ControllingTeamID()
{
	OnTeamControlChanged.Broadcast(ControllingTeamID);
}

void AFlagZone::UpdateCaptureProgress()
{
	if (TeamACount > 0 && TeamBCount > 0)
	{
		// Contested: No progress
		UE_LOG(LogTemp, Log, TEXT("Flag %s is contested."), *GetName());
	}
	else if (TeamACount > 0)
	{
		// Team A capturing
		CaptureProgressInt = FMath::Clamp(CaptureProgressInt + TeamACount * CapturePointPerPlayer, -ScoringThreshold, ScoringThreshold);
	}
	else if (TeamBCount > 0)
	{
		// Team B capturing
		CaptureProgressInt = FMath::Clamp(CaptureProgressInt - TeamBCount * CapturePointPerPlayer, -ScoringThreshold, ScoringThreshold);
	}

	if (CaptureProgressInt == ScoringThreshold && !bTeamAOwned)
	{
		HandleChange(250);
		bTeamAOwned = true;
		bTeamBOwned = false;
		StartScoring(0); // Team A owns the flag
	}
	else if (CaptureProgressInt == -ScoringThreshold && !bTeamBOwned)
	{
		HandleChange(250);
		bTeamBOwned = true;
		bTeamAOwned = false;
		StartScoring(1); // Team B owns the flag
	}
	else if (CaptureProgressInt == 0 && (bTeamAOwned || bTeamBOwned))
	{
		HandleChange(50);
		bTeamAOwned = false;
		bTeamBOwned = false;
		StopScoring();
	}

	UE_LOG(LogTemp, Log, TEXT("Capture progress %i."), CaptureProgressInt);
	OnCaptureProgressIntChanged.Broadcast(CaptureProgressInt);
}

void AFlagZone::UpdatePlayerCountInTeam()
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

	if(TeamACount + TeamBCount > 0)
	{
		if (!GetWorldTimerManager().IsTimerActive(FlagUpdateHandle))
		{
			GetWorldTimerManager().SetTimer(FlagUpdateHandle, this, &AFlagZone::UpdateCaptureProgress, CaptureInterval, true);
		}
	}
	else
	{
		GetWorldTimerManager().ClearTimer(FlagUpdateHandle);
	}
}


void AFlagZone::NotifyTeamControlChanged() const
{
	OnTeamControlChanged.Broadcast(ControllingTeamID);
}

/*
void AFlagZone::OnActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if(!HasAuthority())
		return;
	
	if (OtherActor && OtherActor->IsA(ACharacter::StaticClass()))
	{
		PlayersInZone.Add(OtherActor);
	}

	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		if (AGamePlayerState* PlayerState = Cast<AGamePlayerState>(Character->GetPlayerState()))
		{
			if(const int32 TeamID = PlayerState->GetTeamID(); ControllingTeamID != TeamID)
			{
				//start update timer
				GetWorldTimerManager().SetTimer(FlagUpdateHandle, this, &AFlagZone::UpdateCaptureProgress, CaptureUpdateDeltaTime, true);
			}
		}
	}
	
}

void AFlagZone::OnActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if(!HasAuthority())
		return;
	
	if (OtherActor && OtherActor->IsA(ACharacter::StaticClass()))
	{
		PlayersInZone.Remove(OtherActor);
	}

	if(PlayersInZone.Num()<=0)
	{
		//remove update
		GetWorldTimerManager().ClearTimer(FlagUpdateHandle);
	}
}*/

void AFlagZone::StartScoring(int32 TeamID)
{
	if (ControllingTeamID != TeamID)
	{
		ControllingTeamID = TeamID;
		NotifyTeamControlChanged();
	}
}

void AFlagZone::StopScoring()
{
	ControllingTeamID = -1;
	GetWorldTimerManager().ClearTimer(ScoringTimerHandle);
	NotifyTeamControlChanged();
}

void AFlagZone::OnRep_CaptureProgressInt()
{
	OnCaptureProgressIntChanged.Broadcast(CaptureProgressInt);
}

void AFlagZone::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void AFlagZone::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

void AFlagZone::HandleChange(int Point)
{
	for (AActor* Player : PlayersInZone)
	{
		if (ACharacter* Character = Cast<ACharacter>(Player))
		{
			if (AGamePlayerState* PlayerState = Cast<AGamePlayerState>(Character->GetPlayerState()))
			{
				PlayerState->AddScore(Point);
			}
		}
	}
}