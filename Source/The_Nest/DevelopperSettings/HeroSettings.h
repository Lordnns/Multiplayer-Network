// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "HeroSettings.generated.h"

USTRUCT(BlueprintType)
struct FHeroData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hero")
	FGameplayTag HeroTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hero")
	TSoftObjectPtr<USkeletalMesh> HeroMesh;
};

UCLASS(config=Game, defaultconfig, meta=(DisplayName="Hero Settings"))
class THE_NEST_API UHeroSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(config, EditAnywhere, Category="Heroes")
	TArray<FHeroData> HeroList;

	UFUNCTION()
	virtual FName GetCategoryName() const override { return TEXT("Game"); }
};
