// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PlayerUIWidget.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Widgets/PlayerScoreUIWidget.h"

void UPlayerUIWidget::SetHPBlock(float NewHP)
{
	if (HPBlock)
		HPBlock->SetText(FText::FromString(FString::SanitizeFloat(NewHP, 0))); //Set in Text box float to string and show integer only
}

void UPlayerUIWidget::SetNewHidenHPBlock(bool NewHiden)
{
	if (HPBlock) //CheckIsValid
	{
		//Old
		//First simple select for var
		/*ESlateVisibility NewVivibility = ESlateVisibility::Visible; //SetNewHidenVar
		if (!NewHiden)
			NewVivibility = ESlateVisibility::Hidden;*/

		UHorizontalBox* ParentBox = Cast<UHorizontalBox>(HPBlock->GetParent()); //I use parent Horisantal box in BP, so here is magic check!!!
		if (ParentBox)//Check is valid
			ParentBox->SetVisibility((!NewHiden) ? ESlateVisibility::Hidden : ESlateVisibility::Visible); //SetNewVisibility
	}
}

void UPlayerUIWidget::ShowScoreBlock(bool NewHiden)
{
	if (VBoxScore)
		VBoxScore->SetVisibility((NewHiden) ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
}

void UPlayerUIWidget::AddNewScoreWidget(UPlayerScoreUIWidget* NewScoreWidget)
{
	if (NewScoreWidget)
	{
		ScoreWidgetsArray.Add(NewScoreWidget);
		VBoxScore->AddChild(NewScoreWidget);
	}
}

void UPlayerUIWidget::RemoveScoreWidget(UPlayerScoreUIWidget* ScoreWidgetToRemove)
{
	if (ScoreWidgetToRemove && ScoreWidgetsArray.Find(ScoreWidgetToRemove))
	{
		ScoreWidgetToRemove->RemoveFromParent();
		ScoreWidgetsArray.Remove(ScoreWidgetToRemove);
	}
}
