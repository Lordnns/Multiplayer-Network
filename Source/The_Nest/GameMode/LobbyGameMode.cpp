// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "The_Nest/GameState/LobbyGameState.h"
#include "..\Player\PlayerController\GamePlayerController.h"
#include "The_Nest/DevelopperSettings/HeroSettings.h"
#include "The_Nest/Player/Character/ThirdPersonCharacter.h"
#include "The_Nest/Player/PlayerController/LobbyPlayerController.h"
#include "The_Nest/Player/PlayerState/LobbyPlayerState.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if(GameState->PlayerArray.Num() > MaxPlayerPerServer)
	{
		NewPlayer->Destroy();
	}

	ALobbyGameState* GS = GetGameState<ALobbyGameState>();
	if (GS)
	{
		if(ALobbyPlayerState* PS = NewPlayer->GetPlayerState<ALobbyPlayerState>())
		{
			GS->Multicast_PlayerJoinMessage(PS->PlayerName.ToString());
		}
		
	}

	if(GetNumPlayers() == NumberOfPlayerToStartTimer)
	{
		StartTravelTimer();
	}
}

void ALobbyGameMode::StartTravelTimer()
{
	float TimerEndTime = CalculateEndOfTimer();

	if(ALobbyGameState* Gamestate = GetGameState<ALobbyGameState>())
	{
		Gamestate->LobbyEndTime = TimerEndTime;
		Gamestate->LobbyEndTimeChanged();
	}
	
	GetWorldTimerManager().SetTimer(
		LobbyCountDown, this, &ALobbyGameMode::StartGame,TimeBeforeGameStart, false);
		
}

ALobbyGameMode::ALobbyGameMode()
{
	TimeBeforeGameStart = 30;
	NumberOfPlayerToStartTimer = 2;
	MaxPlayerPerServer = 6;
	bTimerIsSet = false;

	const UHeroSettings* HeroSettings = GetDefault<UHeroSettings>();
	if (HeroSettings && HeroSettings->HeroList.Num() > 0)
	{
		DefaultSkinTag = HeroSettings->HeroList[0].HeroTag;
	}
}


void ALobbyGameMode::StartGame()
{
	if(HasAuthority())
	{
		
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PlayerController = Iterator->Get();
			if (PlayerController)
			{
				ALobbyPlayerController* pc = Cast<ALobbyPlayerController>(PlayerController);
				if(pc)
				{
					if(pc->IsLocalController())
						pc->ShowLoadingScreen();
					else
						pc->ClientShowLoadingScreen();
				}
			}
		}
		FTimerHandle StartGameTimerHandle;
		GetWorldTimerManager().SetTimer(StartGameTimerHandle, this, &ALobbyGameMode::TravelToGameLevel, 0.5f, true);
	}

}

int32 ALobbyGameMode::GetWinningChoice(int32 TeamDeathmatchVotes, int32 DominationVotes, int32 HardpointVotes)
{
	// Store votes in an array
	TArray<int32> Votes = { TeamDeathmatchVotes, DominationVotes, HardpointVotes };

	// Find the index of the highest vote count
	int32 WinningChoice = 0; // Initialize with the first choice
	int32 HighestVotes = Votes[0];

	for (int32 i = 1; i < Votes.Num(); i++)
	{
		if (Votes[i] > HighestVotes)
		{
			HighestVotes = Votes[i];
			WinningChoice = i;
		}
	}
	
	return WinningChoice;
}

FString ALobbyGameMode::HandleGameModeSelection(int32 GameModeChoice)
{
	switch (GameModeChoice)
	{
	case 0: // TDM
		UE_LOG(LogTemp, Log, TEXT("Selected Game Mode: Team Deathmatch"));
		return TEXT("?game=TeamDeathMatch");

	case 1: // Domination
		UE_LOG(LogTemp, Log, TEXT("Selected Game Mode: Domination"));
		return TEXT("?game=Domination");

	case 2: // Hardpoint
		UE_LOG(LogTemp, Log, TEXT("Selected Game Mode: Hardpoint"));
		return TEXT("?game=HardPoint");

	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid Game Mode Choice: %d"), GameModeChoice);
		return TEXT("?game=DefaultGameMode");
	}
}

void ALobbyGameMode::TravelToGameLevel()
{
	if (!GameWorldPath.IsNull())
	{
		if(ALobbyGameState* GS = GetGameState<ALobbyGameState>())
		{
			int32 TeamDeathmatchVotes = 0;
			int32 DominationVotes = 0;
			int32 HardpointVotes = 0;
			GS->GetCurrentVoteCounts(TeamDeathmatchVotes, DominationVotes, HardpointVotes);
			const int Choice = GetWinningChoice(TeamDeathmatchVotes, DominationVotes, HardpointVotes);
			const FString GameModeParam = HandleGameModeSelection(Choice);
			
			const FString LevelPath = FPackageName::ObjectPathToPackageName(GameWorldPath.ToString()) + GameModeParam;
			GetWorld()->ServerTravel(LevelPath, false);
		}
	}
}

float ALobbyGameMode::CalculateEndOfTimer() const
{
	return GetWorld()->GetTimeSeconds() + TimeBeforeGameStart;
}


FString ALobbyGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	const FString PlayerName = UGameplayStatics::ParseOption(Options, TEXT("PlayerName"));
	
	if (ALobbyPlayerState* PS = NewPlayerController->GetPlayerState<ALobbyPlayerState>())
	{
		PS->PlayerName = FText::FromString(PlayerName);
	}

	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (!Exiting) return;

	// Get the PlayerState of the disconnecting player
	ALobbyPlayerState* ExitingPlayerState = Cast<ALobbyPlayerState>(Exiting->PlayerState);
	if (!ExitingPlayerState) return;

	UE_LOG(LogTemp, Log, TEXT("Player %s has left the game."), *ExitingPlayerState->PlayerName.ToString());

	// Remove the player from their team in the GameState
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();
	if (GS)
	{
		GS->Multicast_PlayerLeftMessage(ExitingPlayerState->PlayerName.ToString());
		ExitingPlayerState->VoteChoice = 4;
		GS->SetCurrentVoteCounts();
		GS->RemoveSkinFromSlectedSkins(ExitingPlayerState->GetSelectedSkin());
		if(GetNumPlayers() - 1 < NumberOfPlayerToStartTimer)
		{
			GetWorldTimerManager().ClearTimer(LobbyCountDown);
			GS->StopLobbyTimer();
		}
	}
	
	
}

void ALobbyGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	if(GetNumPlayers() >= NumberOfPlayerToStartTimer)
	{
		StartTravelTimer();
	}
	
}

bool ALobbyGameMode::ValidateAndRegisterSkin(FGameplayTag& SkinTag, ALobbyPlayerState* RequestingPlayer)
{
	ALobbyGameState* Gs = GetGameState<ALobbyGameState>();
	
	if (Gs->GetSelectedSkins().Contains(SkinTag))
	{
		return false;
	}
	
	if (RequestingPlayer)
	{
		if(SkinTag != DefaultSkinTag)
			Gs->AddSkinnToSlectedSkins(SkinTag);
		RequestingPlayer->SetSelectedSkin(SkinTag);
		const AThirdPersonCharacter* CharacterPawn = Cast<AThirdPersonCharacter>(RequestingPlayer->GetPawn());
		CharacterPawn->SkinComponent->ChangeSkin(SkinTag);
		return true;
	}

	return false;
}

