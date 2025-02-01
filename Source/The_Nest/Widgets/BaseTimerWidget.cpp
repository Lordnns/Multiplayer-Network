// Fill out your copyright notice in the Description page of Project Settings.


#include "The_Nest/Widgets/BaseTimerWidget.h"

void UBaseTimerWidget::SetRemainingTime(float rTime)
{
	RemainingTime = rTime;

}

void UBaseTimerWidget::StartTimer()
{
	bIsTimerActive = true;
}

void UBaseTimerWidget::StopTimer()
{
	bIsTimerActive = false;
}

void UBaseTimerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(bIsTimerActive)
	{
		RemainingTime -= InDeltaTime;
		OnTimerUpdatedDelegate.Broadcast(RemainingTime);
	}
}

void UBaseTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

