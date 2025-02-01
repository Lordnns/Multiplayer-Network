// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseTimerWidget.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerUpdated, float, time);
UCLASS()
class THE_NEST_API UBaseTimerWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY()
	float RemainingTime;
	
	UPROPERTY()
	bool bIsTimerActive = false;

	UFUNCTION()
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintAssignable)
	FOnTimerUpdated OnTimerUpdatedDelegate;

public:
	
	UFUNCTION(BlueprintCallable)
	void SetRemainingTime(float rTime);

	UFUNCTION(BlueprintCallable)
	void StartTimer();

	UFUNCTION(BlueprintCallable)
	void StopTimer();
	


	
};
