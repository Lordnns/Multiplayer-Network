// Fill out your copyright notice in the Description page of Project Settings.


#include "GameGameMode.h"

#include "EngineUtils.h"
#include "../../../Plugins/ElectronicNodes/Source/ElectronicNodes/Private/Lib/HotPatch.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "The_Nest/DevelopperSettings/HeroSettings.h"
#include "The_Nest/GameState/GameGameState.h"
#include "The_Nest/GameState/LobbyGameState.h"
#include "The_Nest/Player/Character/ThirdPersonCharacter.h"
#include "The_Nest/Player/PlayerState/GamePlayerState.h"

AGameGameMode::AGameGameMode()
{
	MaxGameTime = 300;
	RespawnTime = 3;
	ScoreToWin = 0;
	TeamCount = 2;
	EndGameDelayTime = 10;
	MaxPlayerPerServer = 6;
	PlayerPointPerKill = 0;
	PlayerPointPerRelevantKill = 0;

}

void AGameGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
}

void AGameGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	PlayersArrivedCount++;

	AGameGameState* GS = GetGameState<AGameGameState>();
	if(!GS) return;

	if(AGamePlayerState* PS = C->GetPlayerState<AGamePlayerState>())
	{
		GS->AddSkinnToSlectedSkins(PS->GetSelectedSkin());
	}

	const int TotalPlayers = GS->PlayerArray.Num();
	if(PlayersArrivedCount == TotalPlayers)
	{
		DistributeTeams();
	}
	
}

void AGameGameMode::CheckWinCondition()
{
	AGameGameState* GS = GetGameState<AGameGameState>();
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

void AGameGameMode::MaxTimeReached()
{
	AGameGameState* GS = GetGameState<AGameGameState>();
	if (!GS) return;

	const TArray<int32>& TeamScores = GS->GetAllTeamScores();

	// If no teams or no scores, end as draw.
	if (TeamScores.Num() == 0)
	{
		// Handle draw logic
		HandleDraw();
		return;
	}

	// Find the highest scoring team
	int32 HighestScore = -1;
	int32 WinningTeamID = -1;
	bool bTie = false;

	for (int32 TeamIndex = 0; TeamIndex < TeamScores.Num(); ++TeamIndex)
	{
		if (TeamScores[TeamIndex] > HighestScore)
		{
			HighestScore = TeamScores[TeamIndex];
			WinningTeamID = TeamIndex;
			bTie = false; // reset tie flag whenever a new high score is found
		}
		else if (TeamScores[TeamIndex] == HighestScore)
		{
			// If another team matches the highest score, we have a tie
			bTie = true;
		}
	}

	if (bTie)
	{
		// More than one team shares the top score
		HandleDraw();
	}
	else
	{
		// One clear winner
		HandleTeamWin(WinningTeamID);
	}
}

void AGameGameMode::HandlePlayerKilled(AGamePlayerState* KillerPS, AGamePlayerState* VictimPS)
{
	if (!KillerPS || !VictimPS) return;

	// Increase Killer's kills and Victim's deaths
	VictimPS->AddDeath();

	AGameGameState* GS = GetGameState<AGameGameState>();
	if (GS)
	{
		// Check if victim and killer are on the same team
		if (KillerPS->GetTeamID() == VictimPS->GetTeamID())
		{

			KillerPS->RemoveKill();

			// Reduce the team's score by ScoreByKill
			//Need to be in TDM Not Base
			/*
			int32 CurrentScore = GS->GetTeamScore(KillerPS->GetTeamID());
			GS->SetTeamScore(KillerPS->GetTeamID(), CurrentScore - ScoreByKill);
			*/
			
		}
		else
		{
			KillerPS->AddKill();

			//Need to be in TDM Not Base
			/*
			int32 NewScore = GS->GetTeamScore(KillerPS->GetTeamID()) + ScoreByKill;
			GS->SetTeamScore(KillerPS->GetTeamID(), NewScore);*/
		}
	}
}


void AGameGameMode::RespawnPlayer(AGamePlayerController* PC)
{
	if (!PC)
		return;
	
	const FString TeamIDString = FString::FromInt(Cast<AGamePlayerState>(PC->PlayerState)->GetTeamID());
	const AActor* SpawnPoint = FindRandomPlayerStart(PC, TeamIDString);

	if (!SpawnPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid spawn point found."));
		return;
	}

	// Destroy the existing pawn, if any
	if (APawn* ExistingPawn = PC->GetPawn())
	{
		ExistingPawn->Destroy();
	}

	// Spawn a new pawn
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = PC;
	SpawnParams.Instigator = PC->GetPawn();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AThirdPersonCharacter* NewPawn = GetWorld()->SpawnActor<AThirdPersonCharacter>(
		DefaultPawnClass, // Replace with your custom pawn class if necessary
		SpawnPoint->GetActorLocation(),
		SpawnPoint->GetActorRotation(),
		SpawnParams
	);

	if (!NewPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn a new pawn."));
		return;
	}

	PC->ResetHealth();
	
	// Possess the new pawn
	PC->Possess(NewPawn);
	
	if(AGamePlayerState* PS = PC->GetPlayerState<AGamePlayerState>())
		PS->SetDeadState(false);
	
	// After respawn, re-enable input
	PC->SetIgnoreMoveInput(false);
	PC->SetIgnoreLookInput(false);
	
}

void AGameGameMode::TeleportPlayer(AGamePlayerController* PC)
{
	if(!PC) return;

	const FString TeamIDString = FString::FromInt(Cast<AGamePlayerState>(PC->PlayerState)->GetTeamID());
	
	const AActor* SpawnPoint = FindRandomPlayerStart(PC, TeamIDString);
	
	if(!SpawnPoint)
	{
		UE_LOG(LogTemp, Log, TEXT("No valid spawn point found."));
	}

	if(AThirdPersonCharacter* Pawn = Cast<AThirdPersonCharacter>(PC->GetPawn()))
	{
		Pawn->TeleportTo(SpawnPoint->GetActorLocation(), SpawnPoint->GetActorRotation());
	}
	
}

AActor* AGameGameMode::FindRandomPlayerStart(AController* Player, const FString& IncomingName)
{
	UWorld* World = GetWorld();
	
	if (IncomingName.IsEmpty())
	{
		return FindPlayerStart(Player, IncomingName);
	}
	
	TArray<AActor*> MatchingStarts;


	// If incoming start is specified, then just use it
	if (!IncomingName.IsEmpty())
	{
		const FName IncomingPlayerStartTag = FName(*IncomingName);
		for (TActorIterator<APlayerStart> It(World); It; ++It)
		{
			APlayerStart* Start = *It;
			if (Start && Start->PlayerStartTag == IncomingPlayerStartTag)
			{
				MatchingStarts.Add(Start);
			}
		}
	}

	for (AActor* Match : MatchingStarts)
	{
		UE_LOG(LogTemp, Log, TEXT("Found matching PlayerStart: %s"), *Match->GetName());
	}


	if (MatchingStarts.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, MatchingStarts.Num() - 1);
		return MatchingStarts[RandomIndex];
	}
	
	UE_LOG(LogTemp, Warning, TEXT("No PlayerStart found with tag: %s"), *IncomingName);
	return FindPlayerStart(Player, IncomingName);
}

float AGameGameMode::GetRespawnTime() const
{
	return  RespawnTime;
}

void AGameGameMode::Multicast_DisablePlayerInputs_Implementation()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			PC->SetIgnoreMoveInput(true);
			PC->SetIgnoreLookInput(true);

			if (AGamePlayerController* GamePC = Cast<AGamePlayerController>(PC))
			{
				GamePC->SetCanInteract(false);
				GamePC->SetCanShowPing(false);
				GamePC->SetCanAim(false);
			}
		}
	}
}

void AGameGameMode::HandleTeamWin(int32 WinningTeamID)
{
	UE_LOG(LogTemp, Log, TEXT("Team %d wins with score %d!"), WinningTeamID, GetGameState<AGameGameState>()->GetTeamScore(WinningTeamID));

	//clear the game timer
	GetWorldTimerManager().ClearTimer(GameTimerHandle);


	BroadcastEndGameScreen(true, WinningTeamID);

	DisableAllPlayerInputs();
	// Start a timer to handle the seamless travel
	GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AGameGameMode::PerformSeamlessTravel, EndGameDelayTime);
}

void AGameGameMode::HandleDraw()
{
	UE_LOG(LogTemp, Log, TEXT("The game ended in a draw."));


	BroadcastEndGameScreen(false, -1);
	
	DisableAllPlayerInputs();
	// Start a timer to handle the seamless travel
	GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AGameGameMode::PerformSeamlessTravel, EndGameDelayTime);

}

void AGameGameMode::BroadcastEndGameScreen(bool bTeamWon, int32 WinningTeamID)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			if (AGamePlayerController* GamePC = Cast<AGamePlayerController>(PC))
			{
				GamePC->Client_ShowEndGameScreen(bTeamWon, WinningTeamID);
			}
		}
	}
}

void AGameGameMode::DisableAllPlayerInputs()
{
	if (HasAuthority()) 
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* PC = It->Get())
			{
				if (AGamePlayerController* GamePC = Cast<AGamePlayerController>(PC))
				{
					GamePC->Client_DisableInputs();
				}
			}
		}
	}
}

void AGameGameMode::PerformSeamlessTravel()
{
	if (!LobbyWorldPath.IsNull())
	{
		const FString LevelPath = FPackageName::ObjectPathToPackageName(LobbyWorldPath.ToString());
		GetWorld()->ServerTravel(LevelPath + "?game=Lobby", false);
	}
}

void AGameGameMode::DistributeTeams()
{
	TeamCount = 2; // Force only 2 teams

	AGameGameState* GS = GetGameState<AGameGameState>();
	if(!GS) return;

	const int TotalPlayers = GS->PlayerArray.Num();
	if (TotalPlayers <= 0) return;

	// Determine how many players go to the first team
	int PlayersPerTeam= TotalPlayers / 2;

	int Count = 0;
	for (APlayerState* PS : GS->PlayerArray)
	{
		AGamePlayerState* GamePS = Cast<AGamePlayerState>(PS);
		if(GamePS)
		{
			int AssignedTeam = (Count < PlayersPerTeam) ? 0 : 1;
			GamePS->SetTeamID(AssignedTeam);
			GS->AddPlayerToTeam(AssignedTeam);

			
			if (AThirdPersonCharacter* Character = Cast<AThirdPersonCharacter>(GamePS->GetPawn()))
			{
				Character->UpdateNameColor();
				Character->UpdatePlayerName();
				if(AGamePlayerController* PC = Cast<AGamePlayerController>(PS->GetPlayerController()))
				{
					TeleportPlayer(PC);
				}

			}

			Count++;
		}
	}

	GS->CreateTeams(TeamCount);
	GS->SetScoreToWin(ScoreToWin);
	GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AGameGameMode::MaxTimeReached, MaxGameTime);
}

void AGameGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	if (!Exiting) return;

	// Get the PlayerState of the disconnecting player
	AGamePlayerState* ExitingPlayerState = Cast<AGamePlayerState>(Exiting->PlayerState);
	if (!ExitingPlayerState) return;

	UE_LOG(LogTemp, Log, TEXT("Player %s has left the game."), *ExitingPlayerState->PlayerName.ToString());

	// Remove the player from their team in the GameState
	AGameGameState* GS = GetGameState<AGameGameState>();
	if (GS)
	{
		int32 TeamID = ExitingPlayerState->GetTeamID();
		GS->RemovePlayerFromTeam(TeamID);
		GS->Multicast_PlayerLeftMessage(ExitingPlayerState->PlayerName.ToString());
		GS->RemoveSkinFromSlectedSkins(ExitingPlayerState->GetSelectedSkin());
	}

	// Additional cleanup logic here (e.g., broadcasting a notification)
}

FString AGameGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	const FString PlayerName = UGameplayStatics::ParseOption(Options, TEXT("PlayerName"));
    
	if (AGamePlayerState* PS = Cast<AGamePlayerState>(NewPlayerController->PlayerState))
	{
		if (!PlayerName.IsEmpty())
		{
			PS->PlayerName = FText::FromString(PlayerName);
			UE_LOG(LogTemp, Log, TEXT("Player %s connected."), *PlayerName);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerName not provided in connection options."));
		}
	}

	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

void AGameGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if(GameState->PlayerArray.Num() > MaxPlayerPerServer)
	{
		NewPlayer->Destroy();
	}
	
	AGameGameState* GS = GetGameState<AGameGameState>();
	if (GS)
	{
		if(AGamePlayerState* PS = NewPlayer->GetPlayerState<AGamePlayerState>())
		{
			GS->Multicast_PlayerJoinMessage(PS->PlayerName.ToString());
			
			const UHeroSettings* HeroSettings = GetDefault<UHeroSettings>();
			if (HeroSettings && HeroSettings->HeroList.Num() > 0)
			{
				// Find the next available skin
				const FGameplayTag SelectedSkin = FindNextAvailableSkin(GS, HeroSettings->HeroList);
				
				GS->AddSkinnToSlectedSkins(SelectedSkin);
				PS->SetSelectedSkin(SelectedSkin);
				const AThirdPersonCharacter* CharacterPawn = Cast<AThirdPersonCharacter>(NewPlayer->GetPawn());
				CharacterPawn->SkinComponent->ChangeSkin(SelectedSkin);
			}
			AssignPlayerToTeam(GS, PS);
		}
	}
}

void AGameGameMode::AssignPlayerToTeam(AGameGameState* GS, AGamePlayerState* PlayerState)
{
	if (!PlayerState && !GS) return;

	// Get current player counts for both teams
	int32 TeamACount = GS->GetPlayerCountInTeam(0);
	int32 TeamBCount = GS->GetPlayerCountInTeam(1);

	// Determine the team with the lowest count
	int32 AssignedTeamID = (TeamACount <= TeamBCount) ? 0 : 1;

	// Set the player's team ID
	PlayerState->SetTeamID(AssignedTeamID);

	// Add the player to the team's count in the GameState
	GS->AddPlayerToTeam(AssignedTeamID);

	if (AGamePlayerController* PC = Cast<AGamePlayerController>(PlayerState->GetPlayerController()))
		TeleportPlayer(PC);

	UE_LOG(LogTemp, Log, TEXT("Player %s assigned to Team %d"), *PlayerState->GetPlayerName(), AssignedTeamID);
}

void AGameGameMode::HandleDeath(AGamePlayerController* HitPlayerController, AThirdPersonCharacter* HitPlayer)
{
	HitPlayer->SetIsDead(true);

	HitPlayerController->SetIgnoreMoveInput(true);
	HitPlayerController->SetIgnoreLookInput(true);

	// Set a timer to handle respawn after RespawnTime seconds
	FTimerHandle UniqueRespawnTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		UniqueRespawnTimerHandle, 
		[this, HitPlayerController]()
		{
			this->RespawnPlayer(HitPlayerController);
		},  
		RespawnTime, 
		false
	);
	
}

FGameplayTag AGameGameMode::FindNextAvailableSkin(AGameGameState* CurrentGameState, const TArray<FHeroData>& HeroList)
{
	// Ensure the HeroList has more than one element
	if (HeroList.Num() <= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("HeroList does not contain enough skins to skip the default one."));
		return HeroList[0].HeroTag;
	}

	// Start the loop at index 1 to skip the first/default element
	for (int32 Index = 1; Index < HeroList.Num(); ++Index)
	{
		const FHeroData& Hero = HeroList[Index];
		if (!CurrentGameState->GetSelectedSkins().Contains(Hero.HeroTag))
		{
			return Hero.HeroTag; // Return the first available skin that is not taken
		}
	}

	// No available skins found
	UE_LOG(LogTemp, Warning, TEXT("No available skins found for the joining player."));
	return HeroList[0].HeroTag;
}

