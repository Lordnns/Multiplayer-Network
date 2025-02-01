#include "GamePlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "The_Nest/GameMode/GameGameMode.h"
#include "The_Nest/Player/PlayerState/GamePlayerState.h"
#include "The_Nest/Player/Character/ThirdPersonCharacter.h"
#include "The_Nest/Widgets/EndGameWidget.h"

class AGamePlayerState;

AGamePlayerController::AGamePlayerController()
{
	CurrentInteractable = nullptr;
	PingCheckCooldown = 10.0f;
	PingPerSecond = 10;
	bCanShowPing = true;
	MaxHealth = 1.0f;
	Health = MaxHealth;
	RegenStartHealth = 0.0f;
	RegenTargetHealth = 0.0f;
	RegenTimeElapsed = 0.0f;
	bCanInteract = true;
	bCanShowPing = true;
	
}

void AGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAction("Interact", IE_Pressed, this, &AGamePlayerController::Interact);
	InputComponent->BindAction("ShowPing", IE_Pressed, this, &AGamePlayerController::ShowPing);
	InputComponent->BindAction("ShowPing", IE_Released, this, &AGamePlayerController::HidePing);
}

void AGamePlayerController::Client_ShowHitMarker_Implementation(bool bIsDead)
{
	OnConfirmedHitDelegate.Broadcast(bIsDead);
}

void AGamePlayerController::Client_ShowEndGameScreen_Implementation(bool bTeamWon, int32 WinningTeamID)
{

	//add end game widget
	if (EndGameUserWidget) 
	{
		UUserWidget* EndGameWidget = CreateWidget<UUserWidget>(this, EndGameUserWidget);
		if (EndGameWidget)
		{
			EndGameWidget->AddToViewport();
			
			if (UEndGameWidget* CastedWidget = Cast<UEndGameWidget>(EndGameWidget))
			{
				CastedWidget->UpdateEndGameScreen(bTeamWon, WinningTeamID);
			}
		}
	}

	//remove crosshair
	OnCrossHairToggleDelegate.Broadcast(false);

	
}

void AGamePlayerController::SetInteractable(AInteractSystem* NewInteractable)
{
	CurrentInteractable = NewInteractable;
	if (CurrentInteractable)
		ShowInteractionUI(CurrentInteractable);
	else
		HideInteractionUI();
}

void AGamePlayerController::UpdateUIPrompt(AInteractSystem* Interactable)
{
	UpdateUIOnDoorStateChanged(Interactable);
}

void AGamePlayerController::ClientUpdateUIPrompt_Implementation(AInteractSystem* Interactable)
{
	UpdateUIOnDoorStateChanged(Interactable);
}

void AGamePlayerController::ClientSetinteractable_Implementation(AInteractSystem* NewInteractable)
{
	SetInteractable(NewInteractable);
}

void AGamePlayerController::Interact()
{

	if(!bCanInteract) return;
	
	if (HasAuthority())  // If this is the server or host
	{
		if (CurrentInteractable)
		{

			CurrentInteractable->Interact(this);
			
		}
	}
	else  // If this is a client, we send an RPC to the server
	{
		if (CurrentInteractable)
		{
			ServerInteract(CurrentInteractable);
		}
		else
	    {
			UE_LOG(LogTemp, Warning, TEXT("No interactable object available."));
		}
	}
}

void AGamePlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AGamePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if(HasAuthority())
	{
		if(AThirdPersonCharacter* PlayerCharacter = Cast<AThirdPersonCharacter>(InPawn))
		{
			PlayerCharacter->ApplySkinOnCharacter();
		}
	}
}

void AGamePlayerController::ServerInteract_Implementation(AInteractSystem* Interactable)
{
	if (Interactable)
	{
		// Server processes the interaction
		UE_LOG(LogTemp, Warning, TEXT("Client interacting with Server to interact with object"));
		Interactable->Interact(this);
		
		ClientReceiveInteractionFeedback(Interactable);
	}
}

void AGamePlayerController::ClientReceiveInteractionFeedback_Implementation(AInteractSystem* Interactable)
{
	// Display feedback or UI update for the client after interaction
	UE_LOG(LogTemp, Warning, TEXT("Interaction complete with: %s"), *Interactable->GetName());
}

void AGamePlayerController::AddHealth(float Heal)
{
	Health =  FMath::Clamp(Health + Heal, 0.0f, 1.0f);
	if(IsLocalPlayerController())
		UpdateBloodUI();
}

float AGamePlayerController::RemoveHealth(float Damage)
{
	Health =  FMath::Clamp(Health - Damage, 0.0f, 1.0f);
	if(IsLocalPlayerController())
	{
		UpdateBloodUI();
	}

	
	StopHealthRegeneration();
	
	if (Health < 1.0f)
	{
		// Start a timer that after X seconds will begin health regen:
		GetWorld()->GetTimerManager().SetTimer(DelayedRegenHandle, this, &AGamePlayerController::BeginHealthRegeneration, DelayBeforeRegenStarts, false);
	}

	return Health;
}

void AGamePlayerController::ResetHealth()
{
	// just to not mess with hp regen
	StopHealthRegeneration();

	Health = MaxHealth;

	//update ui
	if(IsLocalPlayerController())
	{
		UpdateBloodUI();
	}
	

}

void AGamePlayerController::BeginHealthRegeneration()
{
	RegenStartHealth = Health;
	RegenTargetHealth = 1.0f;
	RegenTimeElapsed = 0.0f;
	
	GetWorld()->GetTimerManager().SetTimer(HealthRegenTickHandle, this, &AGamePlayerController::RegenerateHealthTick, RegenTickInterval, true);
}

void AGamePlayerController::RegenerateHealthTick()
{
	RegenTimeElapsed += RegenTickInterval;
	const float Alpha = FMath::Clamp(RegenTimeElapsed / RegenDuration, 0.0f, 1.0f);
	const float NewHealth = FMath::Lerp(RegenStartHealth, RegenTargetHealth, Alpha);

	if (const float HealthDifference = NewHealth - Health; HealthDifference > 0.0f)
		AddHealth(HealthDifference);
	
	if (Alpha >= 1.0f - KINDA_SMALL_NUMBER)
		StopHealthRegeneration();
}

void AGamePlayerController::StopHealthRegeneration()
{
	// Clear any timers related to delayed start or ticking regen
	GetWorld()->GetTimerManager().ClearTimer(DelayedRegenHandle);
	GetWorld()->GetTimerManager().ClearTimer(HealthRegenTickHandle);
}

void AGamePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGamePlayerController, Health);
}

void AGamePlayerController::Client_DisableInputs_Implementation()
{
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);
	SetCanInteract(false);
	SetCanShowPing(false);
	SetCanAim(false);
}

void AGamePlayerController::OnRep_Health()
{
	UpdateBloodUI();
}

