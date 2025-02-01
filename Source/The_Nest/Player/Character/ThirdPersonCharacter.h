// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundCue.h"
#include "The_Nest/The_NestCharacter.h"
#include "The_Nest/ActorComponents/RewindComponent.h"
#include "The_Nest/ActorComponents/SkinComponent.h"
#include "ThirdPersonCharacter.generated.h"

UCLASS()
class THE_NEST_API AThirdPersonCharacter : public AThe_NestCharacter
{
	GENERATED_BODY()

public:
	AThirdPersonCharacter();

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnBulletUpdate, BlueprintReadOnly, Category = "Shooter")
	int BulletInCharger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shooter")
	int BulletPerCharger;
	
	UPROPERTY(EditAnywhere, Category = "Shooter")
	USoundBase* EmptyClipSound;
	
	UPROPERTY(EditAnywhere, Category = "Shooter")
	USoundBase* ReloadSound;

	UPROPERTY(EditAnywhere, Category = "Shooter")
	USoundBase* GunNotReadySound;

	UFUNCTION()
	void OnBulletUpdate();

	float TimeAtLastShot;

	UPROPERTY(EditDefaultsOnly, Category = "Shooter")
	float TimeBetweenShots;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Shooter")
	bool bIsAiming;

	UPROPERTY(EditAnywhere, Category = "Shooter")
	float GunDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UCapsuleComponent* ChestCapsule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UCapsuleComponent* CanonCapsule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UCapsuleComponent* UpperArmCapsule_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UCapsuleComponent* LowerArmCapsule_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UCapsuleComponent* handCapsule_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UCapsuleComponent* UpperArmCapsule_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UCapsuleComponent* LowerArmCapsule_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UCapsuleComponent* handCapsule_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UCapsuleComponent* ThighCapsule_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UCapsuleComponent* ThighCapsule_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UCapsuleComponent* CalfCapsule_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UCapsuleComponent* CalfCapsule_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UCapsuleComponent* FootCapsule_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UCapsuleComponent* FootCapsule_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UCapsuleComponent* HeadCapsule;
	

	UPROPERTY(EditAnywhere, Category = "Shooter")
	float ShotLenght;

	UPROPERTY()
	TMap<FName, UCapsuleComponent*> RewindBones;
	
	UPROPERTY(EditAnywhere, Category = "Aiming")
	float AimInterpSpeed;
	
	UPROPERTY(EditAnywhere, Category = "Aiming")
	float AimUpdateInterval;

	FVector TargetAimPosition;
	UE::Math::TRotator<double> TargetAimRotation;
	
	FTimerHandle AimTimerHandle;
	
	float InterpSpeed;

	UPROPERTY(EditDefaultsOnly, Category = " Shooter")
	float RecoilStrength;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ReloadCanon();

	UFUNCTION(Client, Reliable)
	void Client_NotifyReload(int NewBulletCount);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerShoot(float ShotTime, FVector CameraPosition, FVector CameraForwardVector);

	void HandleClientShoot(float ShotTime, FVector CameraPosition, FVector CameraForwardVector);
	void HandleHostShoot(float ShotTime, FVector CameraPosition, FVector CameraForwardVector);
	void OnHitDetected(AActor* HitActor);
	void OnNoHitDetected();

	UFUNCTION(BlueprintImplementableEvent, Category = "Shooter")
	void StartAim();

	UFUNCTION(BlueprintImplementableEvent, Category = "Shooter")
	void StopAim();

	UFUNCTION(BlueprintImplementableEvent, Category = "Shooter")
	void RemoteAimOn(FVector TargetForwardVector);

	UFUNCTION(BlueprintImplementableEvent, Category = "Shooter")
	void RemoteAimOff();

	UPROPERTY(EditDefaultsOnly, Category ="Shooter")
	USoundCue* ShootingSound;

	UFUNCTION(Server, Reliable)
	void Server_SetIsAiming(bool bNewIsAiming, FVector ForwardVector);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RemoteAim(bool bNewIsAiming, FVector TargetForwardVector);

	UFUNCTION()
	void HandleDeath();
	void OnRespawnTimerExpired();

	UPROPERTY(ReplicatedUsing= OnRep_IsDead, BlueprintReadOnly)
	bool bIsDead;

	UFUNCTION()
	void OnRep_IsDead();

#pragma region MuzzleFlash
	

	UPROPERTY(EditDefaultsOnly, Category = "Shooter")
	UParticleSystem* MuzzleFlash;

	// Socket name for the muzzle
	UPROPERTY(EditDefaultsOnly, Category = "Shooter")
	FName MuzzleSocketName;

	// Function to play the muzzle flash effect
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayMuzzleFlash();
	UFUNCTION()
	void Multicast_PlayMuzzleFlash_Implementation();
	

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Fire();
	UFUNCTION()
	void Server_Fire_Implementation();
	UFUNCTION()
	bool Server_Fire_Validate();

	UFUNCTION()
	void SpawnMuzzleFlash();

#pragma endregion Muzzleflash

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DisableCollisionAndStartAnim(bool NewState);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EnableCollision();

public:
	
	UFUNCTION()
	void SetIsDead(bool NewState);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Player stats")
	void UpdateNameColor();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Player stats")
	void UpdatePlayerName();

	UPROPERTY(BlueprintReadWrite, Category = "Shooter")
	bool bReadyToShoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USkinComponent* SkinComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	URewindComponent* RewindComponent;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "CameraMovement")
	void LookAtPlayer();

	UFUNCTION(BlueprintCallable, Category = "PlayerStyle")
	void ApplySkinOnCharacter();

	virtual void OnRep_PlayerState() override;
	
	UFUNCTION(BlueprintCallable, Category = "Networking")
	void ShootHandCanon(float ShotTime);

	UFUNCTION(BlueprintCallable, Category = "Networking")
	void ReloadHandCanon();

	UFUNCTION()
	TMap<FName, UCapsuleComponent*> GetBones();

	void StartAiming();
	void StopAiming();

	UFUNCTION(Client, Reliable)
	void Client_NotifyHit();

	UFUNCTION(Client, Reliable)
	void Client_ConfirmHit();

private:
	float Alpha;
	FRotator CurrentRotation;
	FRotator TargetRotation;
	
};
 