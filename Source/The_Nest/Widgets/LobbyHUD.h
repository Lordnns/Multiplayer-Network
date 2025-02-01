// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseHUD.h"
#include "BaseTimerWidget.h"
#include "LobbySelectionWidget.h"
#include "LobbyHUD.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API ALobbyHUD : public ABaseHUD
{
	GENERATED_BODY()

	ALobbyHUD();

	UFUNCTION()
	void SetLobbyTimer(float time);

	UFUNCTION()
	void StartLobbyTimer();

	UFUNCTION()
	void StopLobbyTimer();

	virtual void BeginPlay() override;

	UPROPERTY()
	UBaseTimerWidget* TimerWidget;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "UI")
	TSubclassOf<UBaseTimerWidget> TimerWidgetClass;

public:

	UPROPERTY()
	bool bTimerIsSet;
	
};
