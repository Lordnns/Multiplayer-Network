#include "InteractSystem.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerController.h"
#include "..\Player\PlayerController\GamePlayerController.h"


AInteractSystem::AInteractSystem()
{
	
	PrimaryActorTick.bCanEverTick = false;
	
	InteractionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionSphere"));
	InteractionComponent->SetupAttachment(RootComponent);
	Cast<UCapsuleComponent>(InteractionComponent)->InitCapsuleSize(100.f, 96.f); // Temporary can be change later or in editor
	InteractionComponent->SetCollisionProfileName(TEXT("Trigger"));
	
	bReplicates = true;
	
	InteractionComponent->OnComponentBeginOverlap.AddDynamic(this, &AInteractSystem::OnOverlapBegin);
	InteractionComponent->OnComponentEndOverlap.AddDynamic(this, &AInteractSystem::OnOverlapEnd);
	
}

void AInteractSystem::BeginPlay()
{
	Super::BeginPlay();
}


void AInteractSystem::Interact(AGamePlayerController* Interactor)
{
	UE_LOG(LogTemp, Warning, TEXT("Interacted with %s"), *GetName());
}


void AInteractSystem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (HasAuthority())  // Server-side logic
	{
		if (OtherActor && (OtherActor != this))
		{
			APawn* PlayerPawn = Cast<APawn>(OtherActor);
			if (PlayerPawn)
			{
				AGamePlayerController* MyPC = Cast<AGamePlayerController>(PlayerPawn->GetController());
				if (MyPC)
				{
					AddInteractor(MyPC);
					
					if (MyPC->IsLocalController())
						MyPC->SetInteractable(this);
					else 
						MyPC->ClientSetinteractable(this);
				}
			}
		}
	}
}

void AInteractSystem::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority()) // Server-side logic
	{
		if (OtherActor && (OtherActor != this))
		{
			APawn* PlayerPawn = Cast<APawn>(OtherActor);
			if (PlayerPawn)
			{
				AGamePlayerController* MyPC = Cast<AGamePlayerController>(PlayerPawn->GetController());
				if (MyPC)
				{
					RemoveInteractor(MyPC);
					
					if (MyPC->IsLocalController())
						MyPC->SetInteractable(nullptr);
					else
						MyPC->ClientSetinteractable(nullptr);
				}
			}
		}
	}
}

FText AInteractSystem::GetPrompt() const
{
	return Prompt;
}

void AInteractSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AInteractSystem::BindOverlapEvents(UPrimitiveComponent* Component)
{
	if (Component)
	{
		Component->OnComponentBeginOverlap.AddDynamic(this, &AInteractSystem::OnOverlapBegin);
		Component->OnComponentEndOverlap.AddDynamic(this, &AInteractSystem::OnOverlapEnd);
	}
}

void AInteractSystem::AddInteractor(AGamePlayerController* NewInteractor)
{
	if (NewInteractor && !Interactors.Contains(NewInteractor))
		Interactors.Add(NewInteractor);
}

void AInteractSystem::RemoveInteractor(AGamePlayerController* ExitingInteractor)
{
	if (ExitingInteractor && Interactors.Contains(ExitingInteractor))
		Interactors.Remove(ExitingInteractor);
}
