// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlagZone.h"
#include "HardpointZone.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API AHardpointZone : public AFlagZone
{
	GENERATED_BODY()

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

protected:
	virtual void UpdatePlayerCountInTeam() override;
	
};
