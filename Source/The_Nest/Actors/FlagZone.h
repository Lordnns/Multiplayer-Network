// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlagZone.generated.h"

UCLASS()
class THE_NEST_API AFlagZone : public AActor
{
	GENERATED_BODY()

public:
	
	// Sets default values for this actor's properties
	AFlagZone();

	// Getter for ControllingTeamID
	UFUNCTION(BlueprintCallable, Category = "FlagZone")
	int32 GetControllingTeamID() const { return ControllingTeamID; }

	void SetFlagName(FName name);

	UFUNCTION()
	virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Team ID that currently controls the flag (-1 means no control)
	UPROPERTY(Replicatedusing = OnRep_ControllingTeamID, VisibleAnywhere, BlueprintReadOnly, Category = "FlagZone")
	int32 ControllingTeamID;

	UFUNCTION()
	void OnRep_ControllingTeamID();

	UPROPERTY(EditAnywhere)
	UPrimitiveComponent* OverlapComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Rules")
	int CapturePointPerPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Rules")
	float CaptureInterval;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Rules")
	int ScoringThreshold;
	

	//UFUNCTION(BlueprintCallable)
	//void OnActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	
	//UFUNCTION(BlueprintCallable)
	//void OnActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	void StartScoring(int32 TeamID);
	void StopScoring();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flag")
	float DecaySpeed;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Flag")
	FName FlagName;

	// Capture progress
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flag")
	float CaptureProgress;

	UFUNCTION()
	void OnRep_CaptureProgressInt();
	
	// Capture progress
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_CaptureProgressInt, Category = "Flag")
	int CaptureProgressInt;

	// Capture speed multiplier (affected by the number of players in the zone)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flag")
	float BaseCaptureSpeed;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCaptureProgressChanged, float, Percent, int32, TeamId);
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCaptureProgressChanged OnCaptureProgressChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCaptureProgressIntChanged, int32, CaptureProgressInt);
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCaptureProgressIntChanged OnCaptureProgressIntChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamControlChanged, int32, ControllingTeamID);
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTeamControlChanged OnTeamControlChanged;

	virtual void UpdatePlayerCountInTeam();

	// Players currently in the zone
	TArray<AActor*> PlayersInZone;

	int TeamACount;
	int TeamBCount;

	FTimerHandle ScoringTimerHandle;

private:

	bool bTeamAOwned = false;
	bool bTeamBOwned = false;

	void HandleChange(int Point);

	FTimerHandle FlagUpdateHandle;

	float CaptureUpdateDeltaTime;
	
	void UpdateCaptureProgress();
	void UpdateContestingTeams();
	
	void NotifyTeamControlChanged() const;



};
