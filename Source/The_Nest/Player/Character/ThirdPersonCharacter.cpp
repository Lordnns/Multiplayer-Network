// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "The_Nest/GameMode/GameGameMode.h"
#include "The_Nest/Player/PlayerController/GamePlayerController.h"
#include "The_Nest/Player/PlayerState/GamePlayerState.h"
#include "The_Nest/Player/PlayerState/LobbyPlayerState.h"


// Sets default values
AThirdPersonCharacter::AThirdPersonCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SkinComponent = CreateDefaultSubobject<USkinComponent>(TEXT("SkinComponent"));
	RewindComponent = CreateDefaultSubobject<URewindComponent>(TEXT("RewindComponent"));

	ChestCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("chest"));
	ChestCapsule->SetupAttachment(GetMesh(), FName(TEXT("chest")));
	ChestCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CanonCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("canon"));
	CanonCapsule->SetupAttachment(GetMesh(), FName(TEXT("Muzzle_01")));
	CanonCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	HeadCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("head"));
	HeadCapsule->SetupAttachment(GetMesh(), FName(TEXT("head")));
	HeadCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UpperArmCapsule_L = CreateDefaultSubobject<UCapsuleComponent>(TEXT("upperarm_l"));
	UpperArmCapsule_L->SetupAttachment(GetMesh(), FName(TEXT("upperarm_l")));
	UpperArmCapsule_L->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UpperArmCapsule_R = CreateDefaultSubobject<UCapsuleComponent>(TEXT("upperarm_r"));
	UpperArmCapsule_R->SetupAttachment(GetMesh(), FName(TEXT("upperarm_r")));
	UpperArmCapsule_R->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LowerArmCapsule_L = CreateDefaultSubobject<UCapsuleComponent>(TEXT("lowerarm_l"));
	LowerArmCapsule_L->SetupAttachment(GetMesh(), FName(TEXT("lowerarm_l")));
	LowerArmCapsule_L->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LowerArmCapsule_R = CreateDefaultSubobject<UCapsuleComponent>(TEXT("lowerarm_r"));
	LowerArmCapsule_R->SetupAttachment(GetMesh(), FName(TEXT("lowerarm_r")));
	LowerArmCapsule_R->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	handCapsule_L = CreateDefaultSubobject<UCapsuleComponent>(TEXT("hand_l"));
	handCapsule_L->SetupAttachment(GetMesh(), FName(TEXT("hand_l")));
	handCapsule_L->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	handCapsule_R = CreateDefaultSubobject<UCapsuleComponent>(TEXT("hand_r"));
	handCapsule_R->SetupAttachment(GetMesh(), FName(TEXT("hand_r")));
	handCapsule_R->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ThighCapsule_L = CreateDefaultSubobject<UCapsuleComponent>(TEXT("thigh_l"));
	ThighCapsule_L->SetupAttachment(GetMesh(), FName(TEXT("thigh_l")));
	ThighCapsule_L->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ThighCapsule_R = CreateDefaultSubobject<UCapsuleComponent>(TEXT("thigh_r"));
	ThighCapsule_R->SetupAttachment(GetMesh(), FName(TEXT("thigh_r")));
	ThighCapsule_R->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CalfCapsule_L = CreateDefaultSubobject<UCapsuleComponent>(TEXT("calf_l"));
	CalfCapsule_L->SetupAttachment(GetMesh(), FName(TEXT("calf_l")));
	CalfCapsule_L->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CalfCapsule_R = CreateDefaultSubobject<UCapsuleComponent>(TEXT("calf_r"));
	CalfCapsule_R->SetupAttachment(GetMesh(), FName(TEXT("calf_r")));
	CalfCapsule_R->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FootCapsule_L = CreateDefaultSubobject<UCapsuleComponent>(TEXT("foot_l"));
	FootCapsule_L->SetupAttachment(GetMesh(), FName(TEXT("foot_l")));
	FootCapsule_L->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FootCapsule_R = CreateDefaultSubobject<UCapsuleComponent>(TEXT("foot_r"));
	FootCapsule_R->SetupAttachment(GetMesh(), FName(TEXT("foot_r")));
	FootCapsule_R->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	RewindBones.Add(TEXT("head"), HeadCapsule);
	RewindBones.Add(TEXT("chest"), ChestCapsule);
	RewindBones.Add(TEXT("upperarm_l"), UpperArmCapsule_L);
	RewindBones.Add(TEXT("upperarm_r"), UpperArmCapsule_R);
	RewindBones.Add(TEXT("lowerarm_l"), LowerArmCapsule_L);
	RewindBones.Add(TEXT("lowerarm_r"), LowerArmCapsule_R);
	RewindBones.Add(TEXT("hand_l"), handCapsule_L);
	RewindBones.Add(TEXT("hand_r"), handCapsule_R);
	RewindBones.Add(TEXT("thigh_l"), ThighCapsule_L);
	RewindBones.Add(TEXT("thigh_r"), ThighCapsule_R);
	RewindBones.Add(TEXT("calf_l"), CalfCapsule_L);
	RewindBones.Add(TEXT("calf_r"), CalfCapsule_R);
	RewindBones.Add(TEXT("foot_l"), FootCapsule_L);
	RewindBones.Add(TEXT("foot_r"), FootCapsule_R);
	RewindBones.Add(TEXT("canon"), CanonCapsule);
	

	ShotLenght = 10000;
	bIsAiming = false;
	AimInterpSpeed = 0.01f;
	AimUpdateInterval = 0.016f;

	MuzzleSocketName = "Muzzle_01";

	bReplicates = true;

	GunDamage = 0.3f;
	RecoilStrength = 2.5f;

	BulletPerCharger = 5;
	BulletInCharger = 5;
	
}

void AThirdPersonCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AThirdPersonCharacter, bIsAiming);
	DOREPLIFETIME(AThirdPersonCharacter, bIsDead);
	DOREPLIFETIME(AThirdPersonCharacter, BulletInCharger);
	
}

// Called when the game starts or when spawned
void AThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	BulletInCharger = BulletPerCharger;
}

void AThirdPersonCharacter::OnBulletUpdate()
{
	if(AGamePlayerController* PC = Cast<AGamePlayerController>(GetOwner()))
	{
		PC->UpdateAmmoUI(BulletInCharger, BulletPerCharger);
	}
	UE_LOG(LogTemp, Log, TEXT("This is called and Bullet = %i"), BulletInCharger);
}

// Called to bind functionality to input
void AThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AThirdPersonCharacter::LookAtPlayer()
{
	if (Controller)
	{
		FRotator ControlRotation = Controller->GetControlRotation();
		ControlRotation.Yaw += 180.0f;
		Controller->SetControlRotation(ControlRotation);
	}
}

void AThirdPersonCharacter::ApplySkinOnCharacter()
{
		if (USkinComponent* SkinComp = SkinComponent)
    	{
    		// Get the PlayerState and check if it has a SkinTag
    		if (ALobbyPlayerState* PS = GetPlayerState<ALobbyPlayerState>())
    		{
    			if (PS->GetSelectedSkin().IsValid())
    			{
    				SkinComp->ChangeSkin(PS->GetSelectedSkin());
    			}
    		}

			if (AGamePlayerState* PS = GetPlayerState<AGamePlayerState>())
			{
				if (PS->GetSelectedSkin().IsValid())
				{
					SkinComp->ChangeSkin(PS->GetSelectedSkin());
				}
			}
    	}
}

void AThirdPersonCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ApplySkinOnCharacter();
	UpdatePlayerName();
	UpdateNameColor();
}


void AThirdPersonCharacter::ShootHandCanon(float ShotTime)
{
	if (HasAuthority())
	{
		if(BulletInCharger > 0)
		{
			if(GetWorld()->GetTimeSeconds() - TimeAtLastShot > TimeBetweenShots)
			{
				BulletInCharger--;
				if(AGamePlayerController* PC = Cast<AGamePlayerController>(GetOwner()))
					PC->UpdateAmmoUI(BulletInCharger, BulletPerCharger);
				HandleHostShoot(ShotTime, GetFollowCamera()->GetComponentLocation(), GetFollowCamera()->GetForwardVector());
				Multicast_PlayMuzzleFlash();
			
				SpawnMuzzleFlash();
				if (Controller)
				{
					FRotator ControlRotation = Controller->GetControlRotation();
					ControlRotation.Pitch += RecoilStrength;
					Controller->SetControlRotation(ControlRotation);
				}
			}
			else
			{
				if(GunNotReadySound)
					UGameplayStatics::PlaySound2D(GetWorld(), GunNotReadySound);
			}
		}
		else
		{
			if(EmptyClipSound)
				UGameplayStatics::PlaySound2D(GetWorld(), EmptyClipSound);
		}
	}
	else
	{
		if(BulletInCharger > 0)
		{
			if(GetWorld()->GetTimeSeconds() - TimeAtLastShot > TimeBetweenShots)
			{
				TimeAtLastShot =  GetWorld()->GetTimeSeconds();
				// Collect all rewind components in the level
				TArray<URewindComponent*> RewindComponents;
				for (TObjectIterator<URewindComponent> It; It; ++It)
				{
					if (It->GetWorld() == GetWorld())
					{
						RewindComponents.Add(*It);
					}
				}

				// Retrieve frames for all players
				TArray<FRewindData> ClientRewindData;
				for (URewindComponent* RewindComp : RewindComponents)
				{
					FRewindData Data;
					Data.Owner = RewindComp->GetOwner();
					const float Time = GetWorld()->GetTimeSeconds();
					RewindComp->SaveFrameCall();
					Data.Frame = RewindComp->GetRewindFrame(Time);
					ClientRewindData.Add(Data);
				}

				// Send shot information and the interpolated frames to the server
				ServerShoot(ShotTime, GetFollowCamera()->GetComponentLocation(), GetFollowCamera()->GetForwardVector(), ClientRewindData);
				Server_Fire();
			
				SpawnMuzzleFlash();
				if (Controller)
				{
					FRotator ControlRotation = Controller->GetControlRotation();
					ControlRotation.Pitch += RecoilStrength;
					Controller->SetControlRotation(ControlRotation);
				}
			}
			else
			{
				if(GunNotReadySound)
					UGameplayStatics::PlaySound2D(GetWorld(), GunNotReadySound);
			}
		}
		else
		{
			if(EmptyClipSound)
				UGameplayStatics::PlaySound2D(GetWorld(), EmptyClipSound);
		}
	}
	
}

void AThirdPersonCharacter::ReloadHandCanon()
{
	if(BulletInCharger == BulletPerCharger) return;
	
	if(HasAuthority())
	{
		BulletInCharger = BulletPerCharger;
		if(AGamePlayerController* PC = Cast<AGamePlayerController>(GetOwner()))
			PC->UpdateAmmoUI(BulletInCharger, BulletPerCharger);
		if(ReloadSound)
			UGameplayStatics::PlaySound2D(GetWorld(), ReloadSound);
	}
	else
	{
		Server_ReloadCanon();
	}
}

TMap<FName, UCapsuleComponent*> AThirdPersonCharacter::GetBones()
{
	return RewindBones;
}

void AThirdPersonCharacter::ServerShoot_Implementation(float ShotTime, FVector CameraPosition, FVector CameraForwardVector, const TArray<FRewindData>& ClientRewindData)
{
	// Retrieve server's rewind frames
	TArray<FFramePackage> ServerRewindFrames;
	for (const FRewindData& Data : ClientRewindData)
	{
		if (Data.Owner && !Data.Owner->IsPendingKillPending()) // Check if the owner is valid and not marked for destruction
		{
			if (URewindComponent* RewindComp = Cast<URewindComponent>(Data.Owner->FindComponentByClass<URewindComponent>()))
			{
				ServerRewindFrames.Add(RewindComp->GetRewindFrame(ShotTime));
			}
		}
	}

	// Compare Client Frames with Server Frames
	bool bValidFrames = true;
	for (int32 i = 0; i < ClientRewindData.Num(); i++)
	{
		const FFramePackage& ClientFrame = ClientRewindData[i].Frame;
		const FFramePackage* ServerFrame = ServerRewindFrames.IsValidIndex(i) ? &ServerRewindFrames[i] : nullptr;
		
		if (ServerFrame)
		{
			for (const auto& ClientPair : ClientFrame.Colliders)
			{
				const FName& BoneName = ClientPair.BoneName;
				const FFrameCollider& ClientCollider = ClientPair.Collider;

				const FFrameCollider* ServerCollider = nullptr;
				for (const auto& ServerPair : ServerFrame->Colliders)
				{
					if (ServerPair.BoneName == BoneName)
					{
						ServerCollider = &ServerPair.Collider;
						break;
					}
				}

				if (ServerCollider)
				{
					// Verify the sent frame aligns with the server's frame within an allowed variation
					if (FVector::Dist(ServerCollider->Location, ClientCollider.Location) > 20.0f || // Position threshold
						FQuat::ErrorAutoNormalize(ServerCollider->Rotation.Quaternion(), ClientCollider.Rotation.Quaternion()) > 0.1f) // Rotation threshold
							{
						bValidFrames = true;// we need to make this better but verify logic should turn this to false when implemented
						break;
							}
				}
			}
		}

		if (!bValidFrames) break;
	}


	// Perform server-side hit detection if the frames are valid
	if (bValidFrames) {
		HandleClientShoot(ShotTime, CameraPosition, CameraForwardVector, ClientRewindData);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("Client frame verification failed."));
	}
}

bool AThirdPersonCharacter::ServerShoot_Validate(float ShotTime, FVector CameraPosition, FVector CameraForwardVector, const TArray<FRewindData>& ClientRewindData)
{
	return true; // Further anti-cheat checks can be added here
}

void AThirdPersonCharacter::Client_NotifyReload_Implementation(int NewBulletCount)
{
	BulletInCharger = NewBulletCount;
	if(AGamePlayerController* PC = Cast<AGamePlayerController>(GetOwner()))
		PC->UpdateAmmoUI(BulletInCharger, BulletPerCharger);
	if(ReloadSound)
		UGameplayStatics::PlaySound2D(GetWorld(), ReloadSound);
}


void AThirdPersonCharacter::Server_ReloadCanon_Implementation()
{
	if(BulletInCharger == BulletPerCharger) { return; }
	
	if(!HasAuthority()) return;
	UE_LOG(LogTemp, Log, TEXT("ServerReloadGun: Before reload, BulletInCharger = %d"), BulletInCharger);

	BulletInCharger = BulletPerCharger;

	UE_LOG(LogTemp, Log, TEXT("ServerReloadGun: After reload, BulletInCharger = %d"), BulletInCharger);

	Client_NotifyReload(BulletInCharger);
}

bool AThirdPersonCharacter::Server_ReloadCanon_Validate()
{
	if(BulletInCharger == BulletPerCharger)
	{
		//return false;
		//Usually we would stop it here for cheating but Replication issue prevent us
		//from doing so so we will make the reload only authoritive and allow
		//player to trigger the rpc anyway but not reload

		return true;
	}
	else
	{
		return true;
	}
}

void AThirdPersonCharacter::HandleClientShoot(float ShotTime, FVector CameraPosition, FVector CameraForwardVector, const TArray<FRewindData>& ValidatedRewindData)
{

	TimeAtLastShot = GetWorld()->GetTimeSeconds();
	BulletInCharger--;
	float TimeOfShot = ShotTime;
    UWorld* World = GetWorld();

    FVector Start = CameraPosition;
    FVector End = CameraPosition + (CameraForwardVector * ShotLenght);

	//DrawDebugLine(World, Start, End, FColor::Blue, false, 1.0f, 0, 2.0f);

    // Perform a raycast including only static objects
    FHitResult HitResult;
    FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
     // Iterate over all rewind components and set bones/capsules to their rewind positions
    for (const FRewindData& Data : ValidatedRewindData)
    {
        for (const auto& ColliderPair : Data.Frame.Colliders)
        {
            const FName& BoneName = ColliderPair.BoneName;
            const FFrameCollider& Collider = ColliderPair.Collider;
        	if (Data.Owner && !Data.Owner->IsPendingKillPending()) // Check if the owner is valid and not marked for destruction
        	{
        		// Get the corresponding capsule component from the actor
        		if (URewindComponent* RewindComp = Cast<URewindComponent>(Data.Owner->FindComponentByClass<URewindComponent>()))
        		{
        			if (UCapsuleComponent* Capsule = RewindComp->ColliderMap.FindRef(BoneName))
        			{
        				// Update the capsule's position and rotation to the rewinded frame
        				Capsule->SetWorldLocation(Collider.Location);
        				Capsule->SetWorldRotation(Collider.Rotation);

        				// Enable collision for raycasting
        				Capsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        				Capsule->SetCollisionObjectType(ECC_GameTraceChannel1); // Custom channel for rewind collisions
        				Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
        				Capsule->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
        			}
        		}
        	}
        }
    }

    // Perform raycast
    bool bHit = World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	
    // Restore collision settings for rewind capsules
	for (const FRewindData& Data : ValidatedRewindData)
	{
		if (Data.Owner && !Data.Owner->IsPendingKillPending()) // Check if the owner is valid and not marked for destruction
		{
			if (URewindComponent* RewindComp = Cast<URewindComponent>(Data.Owner->FindComponentByClass<URewindComponent>()))
			{
				for (const auto& ColliderPair : Data.Frame.Colliders)
				{
					const FName& BoneName = ColliderPair.BoneName;

					// Get the corresponding capsule component from the actor
					UCapsuleComponent* Capsule = nullptr;
					if (AThirdPersonCharacter* Character = Cast<AThirdPersonCharacter>(Data.Owner))
					{
						Capsule = Character->GetBones().FindRef(BoneName);
					}

					if (Capsule)
					{
						// Restore collision settings
						Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					}
				}
			}
		}
    }
	
	if (bHit)
		OnHitDetected(HitResult.GetActor());
	else
		OnNoHitDetected();

}

void AThirdPersonCharacter::HandleHostShoot(float ShotTime, FVector CameraPosition, FVector CameraForwardVector)
{
	TimeAtLastShot = GetWorld()->GetTimeSeconds();
	UWorld* World = GetWorld();

	// Ensure the chest capsule is valid
	if (!RewindBones.Contains(TEXT("canon")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Canon bone not found in HandleHostShoot."));
		return;
	}

	UCapsuleComponent* RaycastCapsule = RewindBones[TEXT("canon")];
	if (!RaycastCapsule)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanonCapsule is null in HandleHostShoot."));
		return;
	}

	// Get the start and end points of the ray from the root capsule
	FVector Start = CameraPosition;
	FVector End = CameraPosition + (CameraForwardVector * ShotLenght);

	//DrawDebugLine(World, Start, End, FColor::Red, false, 1.0f, 0, 2.0f);
	// Collect all rewind components in the level
	TArray<URewindComponent*> RewindComponents;
	for (TObjectIterator<URewindComponent> It; It; ++It)
	{
		if (It->GetWorld() == World)
		{
			RewindComponents.Add(*It);
		}
	}

	// Enable collision for all capsules in rewind components
	for (URewindComponent* RewindComp : RewindComponents)
	{
		if (RewindComp->GetOwner() == this)
		{
			continue; // Skip the owner
		}

		for (const auto& ColliderPair : RewindComp->ColliderMap)
		{
			UCapsuleComponent* Capsule = ColliderPair.Value;

			if (Capsule)
			{
				// Enable collision for raycasting
				Capsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				Capsule->SetCollisionObjectType(ECC_GameTraceChannel1); // Custom channel for rewind collisions
				Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
				Capsule->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
			}
		}
	}

	// Perform the raycast
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // Ignore the owner

	bool bHit = World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

	// Restore collision settings for all capsules in rewind components
	for (URewindComponent* RewindComp : RewindComponents)
	{
		for (const auto& ColliderPair : RewindComp->ColliderMap)
		{
			UCapsuleComponent* Capsule = ColliderPair.Value;

			if (Capsule)
			{
				// Restore collision settings
				Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
	
	// Handle the hit
	if (bHit)
		OnHitDetected(HitResult.GetActor());
	else
		OnNoHitDetected();

}

// Example OnHitDetected and OnNoHitDetected Functions
void AThirdPersonCharacter::OnHitDetected(AActor* HitActor)
{
    // Handle logic when a hit occurs
    UE_LOG(LogTemp, Log, TEXT("Hit actor: %s"), HitActor ? *HitActor->GetName() : TEXT("Unknown"));

	if(AThirdPersonCharacter* HitPlayer = Cast<AThirdPersonCharacter>(HitActor))
	{
		if(AGamePlayerController* HitPlayerController = Cast<AGamePlayerController>(HitPlayer->GetController()))
		{
			Client_ConfirmHit();

			if(const float RemainingHealth = HitPlayerController->RemoveHealth(GunDamage); RemainingHealth - KINDA_SMALL_NUMBER <= 0)
			{
				if (AGameGameMode* GM = Cast<AGameGameMode>(UGameplayStatics::GetGameMode(this)))
				{
					if (AGamePlayerController* PC = Cast<AGamePlayerController>(GetController()))
					{

						AGamePlayerState* KillerPS = Cast<AGamePlayerState>(PC->PlayerState);
						AGamePlayerState* VictimPS = Cast<AGamePlayerState>(HitPlayerController->PlayerState);

						if (KillerPS && VictimPS && !VictimPS->GetDeadState())
						{
							VictimPS->SetDeadState(true);
							GM->HandlePlayerKilled(KillerPS, VictimPS);
							GM->HandleDeath(HitPlayerController, HitPlayer);

							//TODO Multicast pour anim
							
							PC->Client_ShowHitMarker(true);
						}
					}
				}
			}
			else
			{
				HitPlayer->Client_NotifyHit();

				
				if (AGamePlayerController* ShooterPC = Cast<AGamePlayerController>(GetController()))
				{
					ShooterPC->Client_ShowHitMarker(false);
				}
			}
		}
	}
}

void AThirdPersonCharacter::Client_ConfirmHit_Implementation()
{
	
}

void AThirdPersonCharacter::Client_NotifyHit_Implementation()
{
	
}

void AThirdPersonCharacter::OnNoHitDetected()
{
    // Handle logic when no hit occurs
    UE_LOG(LogTemp, Log, TEXT("No hit detected."));
}

void AThirdPersonCharacter::Server_SetIsAiming_Implementation(bool bNewIsAiming, FVector ForwardVector)
{
	bIsAiming = bNewIsAiming;

	Multicast_RemoteAim(bNewIsAiming, ForwardVector);
}

void AThirdPersonCharacter::Multicast_RemoteAim_Implementation(bool bNewIsAiming, FVector TargetForwardVector)
{
	if(!IsLocallyControlled())
	{
		bIsAiming = bNewIsAiming;
		if(bNewIsAiming)
			RemoteAimOn(TargetForwardVector);
		else
			RemoteAimOff();
	}
	
}

void AThirdPersonCharacter::StartAiming()
{
	bIsAiming = true;
	
	if(!HasAuthority())
		Server_SetIsAiming(true, GetFollowCamera()->GetForwardVector());
	else
		Multicast_RemoteAim(bIsAiming, GetFollowCamera()->GetForwardVector());
	
	StartAim();
}

void AThirdPersonCharacter::StopAiming()
{
	bIsAiming = false;

	if(!HasAuthority())
	{
		Server_SetIsAiming(false,GetFollowCamera()->GetForwardVector());
	}

	StopAim();
}

void AThirdPersonCharacter::HandleDeath()
{
	Multicast_DisableCollisionAndStartAnim(false);
	
	//TODO rpc for death screen
}

void AThirdPersonCharacter::SetIsDead(bool NewState)
{
	bIsDead = NewState;
}

void AThirdPersonCharacter::OnRep_IsDead()
{
	//set value for the anim?
}


void AThirdPersonCharacter::Multicast_PlayMuzzleFlash_Implementation()
{
	//spawn for other player.
	if (!IsLocallyControlled())
	{
		SpawnMuzzleFlash();
	}
}

void AThirdPersonCharacter::Server_Fire_Implementation()
{
	Multicast_PlayMuzzleFlash();
}

bool AThirdPersonCharacter::Server_Fire_Validate()
{
	return true;
}

void AThirdPersonCharacter::SpawnMuzzleFlash()
{
	if (MuzzleFlash)
	{
		
		// Ensure the mesh exists and the socket is valid
		if (USkeletalMeshComponent* MeshComp = GetMesh(); MeshComp && MeshComp->DoesSocketExist(MuzzleSocketName))
		{
			// Get the socket's location and transform
			FTransform MuzzleTransform = MeshComp->GetSocketTransform(MuzzleSocketName);

			// Spawn the particle system at the muzzle location
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleTransform);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Muzzle socket or mesh is invalid."));
		}
		
		if(ShootingSound)
			UGameplayStatics::PlaySoundAtLocation(this, ShootingSound, GetActorLocation());
		
	}
}

void AThirdPersonCharacter::Multicast_DisableCollisionAndStartAnim_Implementation(bool NewState)
{
	SetIsDead(NewState);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	
}

void AThirdPersonCharacter::Multicast_EnableCollision_Implementation()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

