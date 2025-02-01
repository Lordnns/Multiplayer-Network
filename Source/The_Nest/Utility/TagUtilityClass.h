// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TagUtilityClass.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API UTagUtilityClass : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	static void GetAllTagsUnderCategory(FGameplayTag CategoryTag, FGameplayTagContainer& OutTags);
};
