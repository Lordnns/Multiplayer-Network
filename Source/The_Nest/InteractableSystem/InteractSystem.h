// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "InteractSystem.generated.h"

class AGamePlayerController;

UCLASS()
class THE_NEST_API AInteractSystem : public AActor , public IInteractable
{
	GENERATED_BODY()

public:
	
	AInteractSystem();

protected:
	
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	TArray<AGamePlayerController*> Interactors;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	class UPrimitiveComponent* InteractionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText Prompt;

	UFUNCTION()
	void AddInteractor(AGamePlayerController* NewInteractor);

	UFUNCTION()
	void RemoveInteractor(AGamePlayerController* ExitingInteractor);
	
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void BindOverlapEvents(UPrimitiveComponent* Component);

public:
	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void Interact(AGamePlayerController* Interactor) override;
	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual FText GetPrompt() const override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
