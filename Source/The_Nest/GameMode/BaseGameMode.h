// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseGameMode.generated.h"

UCLASS()
class THE_NEST_API ABaseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABaseGameMode();
protected:
	UPROPERTY(EditAnywhere)
	int32 MaxPlayerPerServer;
};
