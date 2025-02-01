#pragma once

#include "CoreMinimal.h"
#include "The_Nest/Player/PlayerController/BasePlayerController.h"
#include "The_Nest/InteractableSystem/InteractSystem.h"
#include "GamePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API AGamePlayerController : public ABasePlayerController
{
	GENERATED_BODY()
	
public:
	AGamePlayerController();
	
protected:
	
	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "Player Stats")
	float Health;

	UFUNCTION()
	void OnRep_Health();
	virtual void SetupInputComponent() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction")
	AInteractSystem* CurrentInteractable;
	
	UFUNCTION(BlueprintImplementableEvent, Category="HUD")
	void ShowInteractionUI(AInteractSystem* Interactable);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateUIOnDoorStateChanged(AInteractSystem* Interactable);
	
	UFUNCTION(BlueprintImplementableEvent, Category="HUD")
	void HideInteractionUI();
	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void Interact();
	
	UFUNCTION(Server, Unreliable)
	void ServerInteract(AInteractSystem* Interactable);

	UFUNCTION(Client, Unreliable)
	void ClientReceiveInteractionFeedback(AInteractSystem* Interactable);

	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void ShowPing();
	
	UFUNCTION(BlueprintImplementableEvent)
	void HidePing();

	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateBloodUI();

	void BeginHealthRegeneration();
	void RegenerateHealthTick();
	void StopHealthRegeneration();

	FTimerHandle DelayedRegenHandle;
	FTimerHandle HealthRegenTickHandle;
	float RegenStartHealth;
	float RegenTargetHealth;
	float RegenTimeElapsed;
	float RegenDuration = 2.0f;
	float RegenTickInterval = 0.05f;
	float DelayBeforeRegenStarts = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> EndGameUserWidget;

	UPROPERTY()
	bool bCanInteract;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCrossHairToggle, bool, NewState);

	UPROPERTY(BlueprintAssignable)
	FOnCrossHairToggle OnCrossHairToggleDelegate;
	
public:

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateAmmoUI(int bulletIn, int BulletPer);
	
	
	// Client RPC to update UI prompt on the client side
	UFUNCTION(Client, Unreliable)
	void ClientUpdateUIPrompt(AInteractSystem* Interactable);
	
	UFUNCTION(Client, Unreliable)
	void ClientSetinteractable(AInteractSystem* NewInteractable);

	UFUNCTION(Client, Unreliable)
	void Client_ShowHitMarker(bool bIsDead);

	UFUNCTION(Client, Reliable)
	void Client_ShowEndGameScreen(bool bTeamWon, int32 WinningTeamID);

	UFUNCTION()
	void SetInteractable(AInteractSystem* NewInteractable);

	UFUNCTION()
	void UpdateUIPrompt(AInteractSystem* Interactable);

	void AddHealth(float Heal);

	float RemoveHealth(float Damage);

	UFUNCTION()
	void ResetHealth();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConfirmedHit, bool, Dead);

	UPROPERTY(BlueprintAssignable)
	FOnConfirmedHit OnConfirmedHitDelegate;

	bool GetCanInteract() const { return bCanInteract; }
	void SetCanInteract(bool bInCanInteract) { bCanInteract = bInCanInteract; }

	UFUNCTION(Client, Reliable)
	void Client_DisableInputs();

};
