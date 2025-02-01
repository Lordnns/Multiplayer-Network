// Copyright Epic Games, Inc. All Rights Reserved.

#include "The_NestGameMode.h"
#include "The_NestCharacter.h"
#include "UObject/ConstructorHelpers.h"

AThe_NestGameMode::AThe_NestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
