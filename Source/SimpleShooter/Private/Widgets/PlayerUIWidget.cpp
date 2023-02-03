// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PlayerUIWidget.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Components/HorizontalBox.h"

void UPlayerUIWidget::SetHPBlock(float NewHP)
{
	if (HPBlock)
		HPBlock->SetText(FText::FromString(FString::SanitizeFloat(NewHP, 0))); //Set in Text box float to string and show integer only
}

void UPlayerUIWidget::SetNewHidenHPBlock(bool NewHiden)
{
	if (HPBlock) //CheckIsValid
	{//(FirstPersonCameraComponent) ? FirstPersonCameraComponent->GetComponentLocation() : GetActorLocation()
		/*ESlateVisibility NewVivibility = ESlateVisibility::Visible; //SetNewHidenVar
		if (!NewHiden)
			NewVivibility = ESlateVisibility::Hidden;*/

		UHorizontalBox* ParentBox = Cast<UHorizontalBox>(HPBlock->GetParent()); //I use parent Horisantal box in BP, so here is magic check!!!
		if (ParentBox)//Check is valid
			ParentBox->SetVisibility((!NewHiden) ? ESlateVisibility::Hidden : ESlateVisibility::Visible); //SetNewVisibility
	}
}
