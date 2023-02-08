// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PlayerScoreUIWidget.h"
#include "Components/TextBlock.h"

void UPlayerScoreUIWidget::UpdateScoreInWidget(int NewID, int NewKills, int NewDeath, int NewHeadshots)
{
	SetTextInBlock(Player_ID, NewID);
	SetTextInBlock(Kills, NewKills);
	SetTextInBlock(Deaths, NewDeath);
	SetTextInBlock(HeadShots, NewHeadshots);
}

void UPlayerScoreUIWidget::SetTextInBlock(UTextBlock* TextBlock, int NewValue)
{
	if (TextBlock)
		TextBlock->SetText(FText::FromString(FString::FromInt(NewValue))); //Set in Text box integer
}
