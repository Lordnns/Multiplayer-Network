// Fill out your copyright notice in the Description page of Project Settings.


#include "The_Nest/Widgets/LobbyHUD.h"

#include "The_Nest/Player/PlayerController/LobbyPlayerController.h"

ALobbyHUD::ALobbyHUD()
{
	bTimerIsSet = false;
}

void ALobbyHUD::SetLobbyTimer(float time)
{
	ALobbyPlayerController* pc = Cast<ALobbyPlayerController>(GetOwningPlayerController());
	ENetMode const NetMode = GetWorld()->GetNetMode();
	
	if (NetMode != NM_Client)
	{
		bTimerIsSet = true;
		const float TimerTime = time - pc->GetServerTime();
		TimerWidget->SetRemainingTime(TimerTime);
		StartLobbyTimer();
	}
	else
	{
		if (pc->bTimeSynchronized)
		{
			bTimerIsSet = true;
			const float TimerTime = time - pc->GetServerTime();
			TimerWidget->SetRemainingTime(TimerTime);
			StartLobbyTimer();
			
		}
	}
}

void ALobbyHUD::StartLobbyTimer()
{
	TimerWidget->StartTimer();
}

void ALobbyHUD::StopLobbyTimer()
{
	TimerWidget->StopTimer();
}

void ALobbyHUD::BeginPlay()
{
	Super::BeginPlay();

	//create widgets instance
	
	if (TimerWidgetClass)
	{
		TimerWidget = CreateWidget<UBaseTimerWidget>(GetWorld(), TimerWidgetClass);
		if (TimerWidget)
		{
			TimerWidget->AddToViewport();
		}
	}

	
	ALobbyPlayerController* PlayerController = Cast<ALobbyPlayerController>(GetOwningPlayerController());
	if (PlayerController)
	{
		// Bind to the delegate in the PlayerController
		PlayerController->OnTimerEndValueChangedDelegate.AddDynamic(this, &ALobbyHUD::SetLobbyTimer);
		PlayerController->OnTimerOffDelegate.AddDynamic(this, &ALobbyHUD::StopLobbyTimer);
	}
	

}
