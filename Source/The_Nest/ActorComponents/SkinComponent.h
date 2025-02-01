// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "SkinComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THE_NEST_API USkinComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	USkinComponent();

	UFUNCTION(BlueprintCallable, Category="PawnSettings")
	void ChangeSkin(FGameplayTag Tag);

protected:

	UFUNCTION()
	USkeletalMesh* GetMeshFromTag(FGameplayTag Tag) const;
};
