// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlagPlaceholder.generated.h"

UCLASS()
class THE_NEST_API AFlagPlaceholder : public AActor
{
	GENERATED_BODY()

public:

	AFlagPlaceholder();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Name")
	FName FlagName;

	


};
