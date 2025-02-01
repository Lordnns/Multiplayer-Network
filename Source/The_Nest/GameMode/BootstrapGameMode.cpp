// Fill out your copyright notice in the Description page of Project Settings.


#include "BootstrapGameMode.h"

void ABootstrapGameMode::OpenMainMenuLevel()
{
	if (!MainMenuWorldPath.IsNull())
	{
		const FString LevelPath = FPackageName::ObjectPathToPackageName(MainMenuWorldPath.ToString() + "?game=MainMenu");
		GetWorld()->ServerTravel(*LevelPath, false);
	}
}
