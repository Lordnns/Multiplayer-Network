#include "DoorInteractable.h"

#include "..\Player\PlayerController\GamePlayerController.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"


ADoorInteractable::ADoorInteractable()
{
	bIsDoorOpen = false;
	bReplicates = true;

	ClosedDoorPrompt = FText::FromString("Press interact button to open door.");
	OpenedDoorPrompt = FText::FromString("Press interact button to close door.");

	if(InteractionComponent)
		InteractionComponent->DestroyComponent();

	UBoxComponent* InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(RootComponent);
	InteractionBox->InitBoxExtent(FVector(100.f, 50.f, 50.f)); // Adjust dimensions as needed
	InteractionBox->SetCollisionProfileName(TEXT("Trigger"));

	InteractionComponent = InteractionBox;

	BindOverlapEvents(InteractionComponent);
	
	Prompt = ClosedDoorPrompt;
	
	PrimaryActorTick.bCanEverTick = false;
}

void ADoorInteractable::OnRep_DoorStateChanged_Implementation()
{
	if (bIsDoorOpen)
	{
		Prompt = OpenedDoorPrompt;
		UE_LOG(LogTemp, Warning, TEXT("Door is now open."));
	}
	else
	{
		Prompt = ClosedDoorPrompt;
		UE_LOG(LogTemp, Warning, TEXT("Door is now closed."));
	}
	if (HasAuthority())
	{
		for (AGamePlayerController* Pc : Interactors)
		{
			if (Pc->IsLocalController())
			{
				Pc->UpdateUIPrompt(this);
			}
			else 
			{
				Pc->ClientUpdateUIPrompt(this);
			}
		}
	}
	else
	{
		// Client-side behavior: only update the client's own UI
		AGamePlayerController* MyPC = Cast<AGamePlayerController>(GetWorld()->GetFirstPlayerController());
		if (MyPC && MyPC->IsLocalController())
		{
			MyPC->UpdateUIPrompt(this);  // Update UI on client side once replicated state is received
		}
	}
}

void ADoorInteractable::Interact(AGamePlayerController* Interactor)
{
	
	bIsDoorOpen = !bIsDoorOpen;
	
	OnRep_DoorStateChanged();
	
	UE_LOG(LogTemp, Warning, TEXT("Interacted with the door: %s"), *GetName());
}

void ADoorInteractable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ADoorInteractable, bIsDoorOpen);
	
}

