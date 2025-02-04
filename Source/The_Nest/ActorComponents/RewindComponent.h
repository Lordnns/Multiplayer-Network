// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RewindComponent.generated.h"

class UCapsuleComponent;

USTRUCT()
struct FFrameCollider
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;
	UPROPERTY()
	FRotator Rotation;
	UPROPERTY()
	float HalfHeight;
	UPROPERTY()
	float Radius;
};

USTRUCT()
struct FColliderPair
{
	GENERATED_BODY()

	UPROPERTY()
	FName BoneName;

	UPROPERTY()
	FFrameCollider Collider;
};

USTRUCT()
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time;
	UPROPERTY()
	TArray<FColliderPair> Colliders;
	UPROPERTY()
	FVector ForwardVector;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class THE_NEST_API URewindComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URewindComponent();

	void SaveFrameCall();
	
	FFramePackage GetRewindFrame(float Time);
	void ShowHistory() const;

	UPROPERTY()
	TMap<FName, UCapsuleComponent*> ColliderMap;

protected:

	void SaveFrame(const TMap<FName, class UCapsuleComponent*>& Colliders);
	virtual void BeginPlay() override;

	UFUNCTION()
	void CustomTickFunction();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewind")
	int32 MaxFrames;

private:
	
	FTimerDynamicDelegate eventTest;
	
	UPROPERTY(EditDefaultsOnly)
	float UpdateRewindFrequency;
	
	FTimerHandle UpdateRewindTimerHandle;
	
	TDoubleLinkedList<FFramePackage> History;
};
