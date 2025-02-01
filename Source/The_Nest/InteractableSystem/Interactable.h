// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

class AGamePlayerController;

UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class THE_NEST_API IInteractable
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual void Interact(AGamePlayerController* Interactor) = 0;
	
	UFUNCTION()
	virtual FText GetPrompt() const = 0;
};
