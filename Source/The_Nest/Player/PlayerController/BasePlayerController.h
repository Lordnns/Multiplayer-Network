// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReplicationStartedSignature);

UCLASS()
class THE_NEST_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ABasePlayerController();
	
private:
	UPROPERTY()
	FTimerHandle PingTimerHandle;
	UPROPERTY()
	FTimerHandle WaitTimerHandle;
	UPROPERTY()
	FTimerHandle SyncTimerHandle;
	UPROPERTY()
	TArray<float> RoundTripTimes;
	UPROPERTY()
	int32 PingCount;
	UPROPERTY()
	bool FirstPingDone;

protected:
	
	UFUNCTION(Server, reliable)
	void ServerRequestWorldTime(float ClientTime);

	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintImplementableEvent)
	void TriggerPause();

	void StartAiming();
	void StopAiming();
	
	void Shoot();
	
	void Reload();

	UFUNCTION(Client, reliable)
	void ClientUpdateWorldTime(float ClientTime, float ServerTime);

	UFUNCTION()
	void CalculateServerDelta(float ClientRequestTime, float ServerTime);

	UFUNCTION()
	void SyncTimeWithServer();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Time")
	float ServerDelta;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Time")
	float SyncInterval;

	UPROPERTY(BlueprintReadOnly, Category = "Networking")
	int32 AverageLatency;
	
	UPROPERTY(EditAnywhere, Category = "Networking")
	int32 PingPerSecond;
	
	UPROPERTY()
	float PingTimer;
	
	UPROPERTY(EditAnywhere, Category = "Networking")
	float PingCheckCooldown;

	UPROPERTY(BlueprintReadWrite, Category = "Controls")
	bool bCanShowPing;

	UFUNCTION(Server,  Reliable)
	void ServerSetHost(bool NewHostValue);

	UFUNCTION()
	void BeginLatencyCheck();

	UFUNCTION()
	void StartLatencyCheck();

	UFUNCTION()
	void PingServer();

	UFUNCTION(Server, Unreliable)
	void ServerReceivePing(float ClientTime);

	UFUNCTION(Client, Unreliable)
	void ClientReceivePong(float ClientTime);

	UFUNCTION()
	void CalculateAverageLatency();

	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerLatencyCompleted(int32 const Latency);

	UFUNCTION()
	void ServerLatencyCompleted_Implementation(int32 const Latency);

	UFUNCTION()
	bool ServerLatencyCompleted_Validate(int32 const Latency);

	void WaitForClientsToComplete(bool bIsQuit);
	void CleanupSession(bool bIsQuit);
	void HandleClientAction(bool bIsQuit);
	void StartDeferLogic(bool bIsQuit);
	
	virtual void OnRep_PlayerState() override;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Networking")
	void ReplicationStarted();

	UPROPERTY()
	bool bCanAim;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> MainMenuWorldPath;
	
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> HostLeftWorldPath;


public:

	UPROPERTY(BlueprintReadOnly, Category = "Networking")
	bool bIsHost;

	UPROPERTY(BlueprintReadOnly, Category = "Networking")
	bool bReplicationStarted;
	
	UPROPERTY(BlueprintReadOnly, Category = "Networking")
	bool bTimeSynchronized;

	UPROPERTY(BlueprintAssignable)
	FOnReplicationStartedSignature OnReplicationStarted; 

	UFUNCTION(BlueprintImplementableEvent, Category="HUD")
	void ShowLoadingScreen();

	UFUNCTION(Client, Reliable)
	void ClientShowLoadingScreen();
	
	UFUNCTION(BlueprintCallable, Category = "Time")
	float GetServerTime() const;
	
	bool GetCanAim() const { return bCanAim; }
	
	void SetCanAim(bool bInCanAim) { bCanAim = bInCanAim; }

	bool GetCanShowPing() const { return bCanShowPing; }
	void SetCanShowPing(bool bInCanShowPing) { bCanShowPing = bInCanShowPing; }

	UFUNCTION(BlueprintCallable, Category = "Networking")
	void ReturnToMainMenu();

	UFUNCTION(BlueprintCallable, Category = "Networking")
	void QuitGame();
	
	
};
