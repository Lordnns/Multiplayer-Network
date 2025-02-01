// Fill out your copyright notice in the Description page of Project Settings.
#include "BasePlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "GameMapsSettings.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "The_Nest/GameState/BaseGameState.h"
#include "The_Nest/Player/Character/ThirdPersonCharacter.h"
#include "The_Nest/Player/PlayerState/GamePlayerState.h"

ABasePlayerController::ABasePlayerController()
{
	PingCheckCooldown = 1.0f;
	PingPerSecond = 10;
	PingCount= 0;
	ServerDelta= 0;
	SyncInterval= 5.0f;
	AverageLatency= 0;
	PingTimer= 0;
	bIsHost= true;
	FirstPingDone = false;
	bCanAim = true;
	bCanShowPing = true;
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
	PingTimer = 1.0f / static_cast<float>(PingPerSecond);
	BeginLatencyCheck();
}

void ABasePlayerController::BeginLatencyCheck()
{
	// Get the current network mode
	ENetMode const NetMode = GetWorld()->GetNetMode();

	// Allow only remote clients to send pings (NM_Client)
	if (NetMode != NM_Client)  // Block listen server and dedicated server from sending pings
		return;
	
	bIsHost = false;
	ServerSetHost(bIsHost);
	StartLatencyCheck();
	SyncTimeWithServer();
}

void ABasePlayerController::ServerSetHost_Implementation(bool NewHostValue)
{
	if (ABasePlayerState* PS = GetPlayerState<ABasePlayerState>())
	{
		PS->bIsHost = NewHostValue;
	}
}

void ABasePlayerController::StartLatencyCheck()
{
	GetWorld()->GetTimerManager().ClearTimer(WaitTimerHandle);
	
	PingCount = 0;
	RoundTripTimes.Empty();
	
	GetWorld()->GetTimerManager().SetTimer(PingTimerHandle, this, &ABasePlayerController::PingServer, PingTimer, true);
}

void ABasePlayerController::PingServer()
{
	ServerReceivePing(GetWorld()->GetTimeSeconds());
}

void ABasePlayerController::ServerReceivePing_Implementation(float ClientTime)
{
	ClientReceivePong(ClientTime);
}

void ABasePlayerController::ClientReceivePong_Implementation(float ClientTime)
{
	if (PingCount >= 10)
		return;

	const float ClientTimeTemp = ClientTime;
	
	float const CurrentTime = GetWorld()->GetTimeSeconds();
	float const RTTSeconds = CurrentTime - ClientTimeTemp;
	int32 const RTTMilliseconds = FMath::RoundToInt(RTTSeconds * 1000);  // Convert RTT to milliseconds

	RoundTripTimes.Add(RTTMilliseconds);  // Store RTT as an int32 (milliseconds)

	++PingCount;

	if (PingCount >= 10)
	{
		GetWorld()->GetTimerManager().ClearTimer(PingTimerHandle);

		CalculateAverageLatency();
		
		GetWorld()->GetTimerManager().SetTimer(WaitTimerHandle, this, &ABasePlayerController::StartLatencyCheck, PingCheckCooldown, false);
	}
}

void ABasePlayerController::CalculateAverageLatency()
{
	if (RoundTripTimes.Num() < 10)
		return;

	// Sort the RTT values
	RoundTripTimes.Sort();

	// Remove the 2 smallest and 2 largest values by ignoring the first two and last two
	int32 TotalRTT = 0;
	for (int32 i = 2; i < RoundTripTimes.Num() - 2; ++i)
	{
		TotalRTT += RoundTripTimes[i];
	}

	// Calculate the average of the remaining 6 RTT values (in milliseconds)
	int32 const AverageRTTMilliseconds = TotalRTT / 6;

	// Divide by 2 to get the latency
	int32 const TempAverageLatency = AverageRTTMilliseconds / 2;
	
	AverageLatency = TempAverageLatency;
	
	// Update the server with the calculated latency
	ServerLatencyCompleted(AverageLatency);
}

void ABasePlayerController::ServerLatencyCompleted_Implementation(int32 const Latency)
{
	if (ABasePlayerState* PS = GetPlayerState<ABasePlayerState>())
	{
		PS->AvgLatency = Latency;
		UE_LOG(LogTemp, Log, TEXT("Updated PlayerState with Latency: %d ms"), Latency);
	}
}

bool ABasePlayerController::ServerLatencyCompleted_Validate(int32 const Latency)
{
	return Latency >= 0 && Latency <= 1000;
}

void ABasePlayerController::SyncTimeWithServer()
{
	if(!bIsHost)
	{
		if(!FirstPingDone)
		{
			FirstPingDone = true;
			GetWorldTimerManager().SetTimer(SyncTimerHandle, this, &ABasePlayerController::SyncTimeWithServer, SyncInterval, true);
		}
		
		ServerRequestWorldTime(GetWorld()->GetTimeSeconds());
		
	}
}

void ABasePlayerController::ServerRequestWorldTime_Implementation(float ClientTime)
{
	const float ServerTime = GetWorld()->GetTimeSeconds();
	ClientUpdateWorldTime(ClientTime, ServerTime);
}

void ABasePlayerController::ClientUpdateWorldTime_Implementation(float ClientTime, float ServerTime)
{
	CalculateServerDelta(ClientTime, ServerTime);
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("Shoot", IE_Pressed, this, &ABasePlayerController::Shoot);
	InputComponent->BindAction("Reload", IE_Pressed, this, &ABasePlayerController::Reload);
	InputComponent->BindAction("Aim", IE_Pressed, this, &ABasePlayerController::StartAiming);
	InputComponent->BindAction("Aim", IE_Released, this, &ABasePlayerController::StopAiming);
	InputComponent->BindAction("Pause", IE_Pressed, this, &ABasePlayerController::TriggerPause);
}

void ABasePlayerController::CalculateServerDelta(float ClientRequestTime, float ServerTime)
{
	const float RoundTripTime = GetWorld()->GetTimeSeconds() - ClientRequestTime;
	ServerDelta = ServerTime - (ClientRequestTime + (RoundTripTime / 2));

	if(!bTimeSynchronized && bReplicationStarted)
	{
		bTimeSynchronized = true;
		OnReplicationStarted.Broadcast();
		ReplicationStarted();
	}
    
	bTimeSynchronized = true;
}

float ABasePlayerController::GetServerTime() const
{
	return GetWorld()->GetTimeSeconds() + ServerDelta;
}

void ABasePlayerController::QuitGame()
{
	if (HasAuthority())
	{
		StartDeferLogic(true);
	}
	else
	{
		HandleClientAction(true);
	}
}

void ABasePlayerController::ReturnToMainMenu()
{
	if (HasAuthority())
	{
		StartDeferLogic(false);
	}
	else
	{
		HandleClientAction(false);
	}
}

void ABasePlayerController::HandleClientAction(bool bIsQuit)
{
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            // End the session
            if (SessionInterface->EndSession(NAME_GameSession))
            {
                UE_LOG(LogTemp, Log, TEXT("EndSession initiated successfully."));

                // Wait for the session to end and then destroy it
                FTimerHandle TimerHandle;
                GetWorld()->GetTimerManager().SetTimer(TimerHandle, [SessionInterface, this, bIsQuit]()
                {
                    // Destroy the session
                    SessionInterface->DestroySession(NAME_GameSession,
                        FOnDestroySessionCompleteDelegate::CreateLambda([this, bIsQuit](FName DestroyedSessionName, bool bWasSuccessful)
                        {
                            UE_LOG(LogTemp, Log, TEXT("Session %s destroyed: %s"), *DestroyedSessionName.ToString(), bWasSuccessful ? TEXT("Success") : TEXT("Failure"));

                            // Finalize the action
                            if (bIsQuit)
                            {
                                FGenericPlatformMisc::RequestExit(false); // Quit the game
                            }
                            else
                            {
                                // Travel to the main menu
                                const FString TravelURL = FPackageName::ObjectPathToPackageName(MainMenuWorldPath.ToString());
                                ClientTravel(TravelURL, TRAVEL_Absolute);
                            }
                        }));
                }, 1.0f, false); // Delay to ensure EndSession completes before DestroySession

                return; // Exit early, as cleanup will complete in the delegate
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to initiate EndSession."));
            }
        }
    }

    // Fallback if session interface is unavailable or invalid
    if (bIsQuit)
    {
        FGenericPlatformMisc::RequestExit(false);
    }
    else
    {
        const FString TravelURL = FPackageName::ObjectPathToPackageName(MainMenuWorldPath.ToString());
        ClientTravel(TravelURL, TRAVEL_Absolute);
    }
}

void ABasePlayerController::StartDeferLogic(bool bIsQuit)
{
	if (ABaseGameState* GS = Cast<ABaseGameState>(GetWorld()->GetGameState()))
	{
		// Notify clients to travel to the fallback map
		const FString LevelPath = FPackageName::ObjectPathToPackageName(HostLeftWorldPath.ToString());
		GS->Multicast_TravelToMap(LevelPath);

		// Start the deferred wait logic
		WaitForClientsToComplete(bIsQuit);
	}
}

void ABasePlayerController::WaitForClientsToComplete(bool bIsQuit)
{
	if (ABaseGameState* GS = Cast<ABaseGameState>(GetWorld()->GetGameState()))
	{
		const int32 PlayerCount = GS->PlayerArray.Num();

		if (PlayerCount == 1) // Only the host remains
		{
			UE_LOG(LogTemp, Log, TEXT("All clients have transitioned. Proceeding with session cleanup."));
			CleanupSession(bIsQuit);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Waiting for clients to leave. Current player count: %d"), PlayerCount);

			// Retry after 1 second
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, bIsQuit]()
			{
				WaitForClientsToComplete(bIsQuit);
			}, 1.0f, false);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameState not found. Cannot check player count."));
	}
}

void ABasePlayerController::CleanupSession(bool bIsQuit)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			// Destroy the session
			SessionInterface->DestroySession(NAME_GameSession, 
				FOnDestroySessionCompleteDelegate::CreateLambda([this, bIsQuit](FName SessionName, bool bWasSuccessful)
				{
					UE_LOG(LogTemp, Log, TEXT("Session %s destroyed: %s"), *SessionName.ToString(), bWasSuccessful ? TEXT("Success") : TEXT("Failure"));

					if (bIsQuit)
					{
						FGenericPlatformMisc::RequestExit(false); // Exit the game
					}
					else
					{
						// Travel to the main menu
						const FString TravelURL = FPackageName::ObjectPathToPackageName(MainMenuWorldPath.ToString());
						ClientTravel(TravelURL, TRAVEL_Absolute);
					}
				}));
			return; // Exit the function early, as the callback handles the next steps
		}
	}

	// Fallback behavior if no session exists
	if (bIsQuit)
	{
		FGenericPlatformMisc::RequestExit(false);
	}
	else
	{
		const FString TravelURL = FPackageName::ObjectPathToPackageName(MainMenuWorldPath.ToString());
		ClientTravel(TravelURL, TRAVEL_Absolute);
	}
}

void ABasePlayerController::ClientShowLoadingScreen_Implementation()
{
	ShowLoadingScreen();
}

void ABasePlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	bReplicationStarted = true;

	if(bTimeSynchronized)
	{
		OnReplicationStarted.Broadcast();
		ReplicationStarted();
	}
}

void ABasePlayerController::ReplicationStarted_Implementation()
{
	
}

void ABasePlayerController::StartAiming()
{
	if(!bCanAim) return;
	
	if(AThirdPersonCharacter* CharacterPawn = Cast<AThirdPersonCharacter>(GetPawn()))
	{
		CharacterPawn->StartAiming();
	}
}

void ABasePlayerController::StopAiming()
{
	if(!bCanAim) return;
	
	if(AThirdPersonCharacter* CharacterPawn = Cast<AThirdPersonCharacter>(GetPawn()))
	{
		CharacterPawn->StopAiming();
	}
}

void ABasePlayerController::Shoot()
{
	if(!bCanAim) return;
	
	if(AThirdPersonCharacter* PlayerPawn = Cast<AThirdPersonCharacter>(GetPawn()))
	{
		if(PlayerPawn->bReadyToShoot)
		{
			if (HasAuthority())
				PlayerPawn->ShootHandCanon(GetWorld()->GetTimeSeconds());
			else
				PlayerPawn->ShootHandCanon(GetServerTime());
		}
	}
}

void ABasePlayerController::Reload()
{
	if(AThirdPersonCharacter* PlayerPawn = Cast<AThirdPersonCharacter>(GetPawn()))
	{
		PlayerPawn->ReloadHandCanon();
	}
}