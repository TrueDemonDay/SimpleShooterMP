// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleShooterPlayerController.h"
#include "Widgets/PlayerUIWidget.h"


void ASimpleShooterPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

}

void ASimpleShooterPlayerController::SetGPWidget(float NewHP)
{
	if (PlayerWidgetRef)
	{
		PlayerWidgetRef->SetHPBlock(NewHP);
	}
}

void ASimpleShooterPlayerController::SetHidenHPWidget(bool NewHiden)
{
	PlayerWidgetRef->SetNewHidenHPBlock(NewHiden);
}
