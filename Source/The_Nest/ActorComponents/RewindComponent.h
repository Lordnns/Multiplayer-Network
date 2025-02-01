// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RewindComponent.generated.h"

USTRUCT()
struct FFrameCollider
{
	GENERATED_BODY()

	FVector Location;
	FRotator Rotation;
	float HalfHeight;
	float Radius;
};

USTRUCT()
struct FFramePackage
{
	GENERATED_BODY()

	float Time;
	TMap<FName, FFrameCollider> Colliders;
	FVector ForwardVector;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class THE_NEST_API URewindComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URewindComponent();

	void SaveFrame(const TMap<FName, class UCapsuleComponent*>& Colliders);
	FFramePackage GetRewindFrame(float Time);
	void ShowHistory() const;

	UPROPERTY()
	TMap<FName, UCapsuleComponent*> ColliderMap;

protected:
	
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
