// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "The_Nest/DevelopperSettings/HeroSettings.h"
#include "The_Nest/GameMode/LobbyGameMode.h"
#include "The_Nest/Player/PlayerState/LobbyPlayerState.h"
#include "The_Nest/Widgets/LobbyHUD.h"


void ALobbyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("ShowPing", IE_Pressed, this, &ALobbyPlayerController::EnableDisableFocus);
}

void ALobbyPlayerController::ServerSetPlayerName_Implementation(const FText& Name)
{
	
	if (ALobbyPlayerState* PS = GetPlayerState<ALobbyPlayerState>())
	{
		PS->PlayerName = Name;
	}
	
}

bool ALobbyPlayerController::ServerSetPlayerName_Validate(const FText& Name)
{
	return Name.ToString().Len() <= 10;
}

void ALobbyPlayerController::HandleLobbyTimerStopped()
{
	OnTimerOffDelegate.Broadcast();
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	
	// Get a reference to the custom GameState
	if (ALobbyGameState* GameState = GetWorld()->GetGameState<ALobbyGameState>())
	{
		// Bind the delegate to your function
		if(IsValid(GameState))
		{
			GameState->FOnRemainingTimeChangedDelegate.AddDynamic(this, &ALobbyPlayerController::HandleRemainingTimeChanged);
			GameState->FOnLobbyTimerStoppedDelegate.AddDynamic(this, &ALobbyPlayerController::HandleLobbyTimerStopped);
		}
	
	}
	if(AThirdPersonCharacter* TPC = Cast<AThirdPersonCharacter>(GetPawn()))
	{
		TPC->LookAtPlayer();
	}
}

void ALobbyPlayerController::HandleRemainingTimeChanged(float time)
{
	OnTimerEndValueChangedDelegate.Broadcast(time);
}

void ALobbyPlayerController::ConfirmSkinSelection(FGameplayTag Tag, bool bIsFirstSelection)
{
	if(HasAuthority())
	{
		if(bool Success = ConfirmPlayerSkin(Tag, GetPlayerState<ALobbyPlayerState>(), bIsFirstSelection))
		{
			if(bIsFirstSelection) return;
			OnConfirmedHeroDelegate.Broadcast();
		}
	}
	else
	{
		ServerConfirmPlayerSkin(Tag, GetPlayerState<ALobbyPlayerState>(), bIsFirstSelection);
	}
		
}

void ALobbyPlayerController::SetVote(int Choice)
{
	ALobbyPlayerState* LPS = GetPlayerState<ALobbyPlayerState>();
	ALobbyGameState* LGS = Cast<ALobbyGameState>(GetWorld()->GetGameState());
	
	if(LPS && LGS)
	{
		LPS->VoteChoice = Choice;
		LGS->SetCurrentVoteCounts();
	}
}

void ALobbyPlayerController::ReceiveVote(int Choice)
{
	if(HasAuthority())
		SetVote(Choice);
	else
		ServerSetVote(Choice);
}

void ALobbyPlayerController::ServerSetVote_Implementation(int Choice)
{
	SetVote(Choice);
}

void ALobbyPlayerController::ClientConfirmPlayerSkin_Implementation(bool RequestAccepted, bool bIsFirstSelection)
{
	if(RequestAccepted)
	{
		if(bIsFirstSelection) return;
		OnConfirmedHeroDelegate.Broadcast();
	}
}

void ALobbyPlayerController::ServerConfirmPlayerSkin_Implementation(FGameplayTag Tag, class ALobbyPlayerState* RequestingPlayer, bool bIsFirstSelection)
{
	ClientConfirmPlayerSkin( ConfirmPlayerSkin(Tag, RequestingPlayer, bIsFirstSelection), bIsFirstSelection);
}

bool ALobbyPlayerController::ServerConfirmPlayerSkin_Validate(FGameplayTag Tag, class ALobbyPlayerState* RequestingPlayer, bool bIsFirstSelection)
{
	return true;
}

bool ALobbyPlayerController::ConfirmPlayerSkin(FGameplayTag Tag, class ALobbyPlayerState* RequestingPlayer, bool bIsFirstSelection) const
{
	AGameModeBase* GameMode =  UGameplayStatics::GetGameMode(GetWorld());

	if(ALobbyGameMode* LobbyGameMode = Cast<ALobbyGameMode>(GameMode))
	{
		return LobbyGameMode->ValidateAndRegisterSkin(Tag, RequestingPlayer);
	}

	return false;
	
}

void ALobbyPlayerController::ReplicationStarted_Implementation()
{
	Super::ReplicationStarted_Implementation();

	if(ALobbyHUD* HUD = Cast<ALobbyHUD>(GetHUD()))
	{
		if(!HUD->bTimerIsSet)
		{
			if(ALobbyGameState* GameState = GetWorld()->GetGameState<ALobbyGameState>())
			{
				OnTimerEndValueChangedDelegate.Broadcast(GameState->LobbyEndTime);
			}
		}
	}
}
