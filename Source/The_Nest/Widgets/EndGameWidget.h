// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "EndGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API UEndGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void UpdateEndGameScreen(bool TeamWin, int32 TeamId);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndGameTextUpdate, FText, Message);

	UPROPERTY(BlueprintAssignable)
	FOnEndGameTextUpdate OnEndGameTextUpdateDelegate;
	
};
