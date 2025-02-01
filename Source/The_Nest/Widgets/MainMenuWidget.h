// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class THE_NEST_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable)
	void Host();

	UFUNCTION(BlueprintCallable)
	void Join(const FString& ServerAddress);
	
	UFUNCTION()
	static FText GenerateRandomPlayerName();

	UFUNCTION()
	void OnPlayerNameChanged(const FText& NewText);

	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	FText PlayerName;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* NameInputBox;
};
