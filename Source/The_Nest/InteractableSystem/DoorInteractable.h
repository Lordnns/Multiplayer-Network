// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractSystem.h"
#include "DoorInteractable.generated.h"

UCLASS()
class THE_NEST_API ADoorInteractable : public AInteractSystem
{
	GENERATED_BODY()

public:
	
	ADoorInteractable();

protected:
	
	// Variable to track the door's open/close state
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DoorStateChanged)
	bool bIsDoorOpen;

	// Function called when the door state is replicated
	UFUNCTION(BlueprintNativeEvent)
	void OnRep_DoorStateChanged();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Interaction")
	FText OpenedDoorPrompt;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Interaction")
	FText ClosedDoorPrompt;
	
	virtual void Interact(AGamePlayerController* Interactor) override;
	
};
