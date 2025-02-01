// Fill out your copyright notice in the Description page of Project Settings.


#include "SkinComponent.h"
#include "GameplayTagContainer.h"
#include "The_Nest/DevelopperSettings/HeroSettings.h"
#include "The_Nest/Player/Character/ThirdPersonCharacter.h"


// Sets default values
USkinComponent::USkinComponent()
{
	PrimaryComponentTick.bCanEverTick=false;

}

void USkinComponent::ChangeSkin(FGameplayTag Tag)
{


	ACharacter* OwnerCharacter = Cast<AThirdPersonCharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkinComponent is not attached to a Character!"));
		return;
	}

	USkeletalMeshComponent* MeshComponent = OwnerCharacter->GetMesh();
	if (!MeshComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character has no SkeletalMeshComponent!"));
		return;
	}

	USkeletalMesh* NewMesh = GetMeshFromTag(Tag);
	if (NewMesh)
	{
		MeshComponent->SetSkeletalMesh(NewMesh);
		UE_LOG(LogTemp, Log, TEXT("Skeletal mesh changed successfully."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No skeletal mesh found for tag: %s"), *Tag.ToString());
	}

}

USkeletalMesh* USkinComponent::GetMeshFromTag(FGameplayTag Tag) const
{
	
	const UHeroSettings* HeroSettings = GetDefault<UHeroSettings>();
	if (!HeroSettings)
	{
		UE_LOG(LogTemp, Warning, TEXT("HeroSettings not found!"));
		return nullptr;
	}

	for (const FHeroData& Hero : HeroSettings->HeroList)
	{
		if (Hero.HeroTag == Tag)
		{
			return Hero.HeroMesh.LoadSynchronous();
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("No matching hero for tag: %s"), *Tag.ToString());
	return nullptr;

}
	

