// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "GameFramework//GameModeBase.h"
#include "The_Nest/GameMode/MainMenuGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "The_Nest/GameInstance/MainGameInstance.h"
#include "The_Nest/Player/PlayerState/GamePlayerState.h"

void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    UMainGameInstance* GameInstance = Cast<UMainGameInstance>(GetWorld()->GetGameInstance());
    if (GameInstance)
    {
        // Check if a PlayerName already exists
        if (!GameInstance->PlayerName.IsEmpty())
        {
            PlayerName = GameInstance->PlayerName;
        }
        else
        {
            PlayerName = GenerateRandomPlayerName();
            GameInstance->PlayerName = PlayerName;
        }
    }
    else
    {
        PlayerName = GenerateRandomPlayerName();
    }


    if (NameInputBox)
    {
        NameInputBox->SetText(PlayerName);
        NameInputBox->OnTextChanged.AddDynamic(this, &UMainMenuWidget::OnPlayerNameChanged);
    }
}

void UMainMenuWidget::Host()
{
    if(AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this))
    {
        if(AMainMenuGameMode* MainMenuGameMode = Cast<AMainMenuGameMode>(GameMode))
        {
            MainMenuGameMode->Host(PlayerName);
        }
    }
}

void UMainMenuWidget::Join(const FString& ServerAddress)
{
    if(AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this))
    {
        if(AMainMenuGameMode* MainMenuGameMode = Cast<AMainMenuGameMode>(GameMode))
        {
            MainMenuGameMode->Join(ServerAddress,PlayerName);
        }
    }
}

FText UMainMenuWidget::GenerateRandomPlayerName()
{
    int32 const RandomNumber = FMath::RandRange(1000, 9999);
	
    FNumberFormattingOptions NumberFormatOptions;
    NumberFormatOptions.SetUseGrouping(false);

    return FText::Format(NSLOCTEXT("PlayerNamespace", "PlayerFormat", "Player{0}"), FText::AsNumber(RandomNumber, &NumberFormatOptions));
}

void UMainMenuWidget::OnPlayerNameChanged(const FText& NewText)
{
    // Define the maximum character limit
    constexpr int32 MaxCharacters = 10;

    // Check if the new text exceeds the maximum length
    if (NewText.ToString().Len() > MaxCharacters)
    {
        // Trim the text to the maximum allowed length
        const FString TrimmedText = NewText.ToString().Left(MaxCharacters);
        NameInputBox->SetText(FText::FromString(TrimmedText));
        PlayerName = FText::FromString(TrimmedText); // Update PlayerName to match the trimmed text
    }
    else
    {
        // Update PlayerName if within limit
        PlayerName = NewText;
    }
    
    if (UMainGameInstance* GameInstance = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
        GameInstance->PlayerName = PlayerName;
}
