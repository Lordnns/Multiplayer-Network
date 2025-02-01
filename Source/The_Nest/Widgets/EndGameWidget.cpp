// Fill out your copyright notice in the Description page of Project Settings.


#include "EndGameWidget.h"

void UEndGameWidget::UpdateEndGameScreen(bool TeamWin, int32 TeamId)
{
	FText Message;

	if (TeamWin)
	{

		Message = FText::Format(NSLOCTEXT("EndGame", "TeamWin", "Team {0} has won the game!"), FText::AsNumber(TeamId));
	}
	else
	{

		Message = NSLOCTEXT("EndGame", "GameDraw", "The game ended in a draw.");
	}

	OnEndGameTextUpdateDelegate.Broadcast(Message);

}
