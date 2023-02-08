// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleShooterPlayerController.h"
#include "Widgets/PlayerUIWidget.h"
#include "SimpleShooterGameMode.h"
#include "SimpleShooterPS.h"
#include "Widgets/PlayerScoreUIWidget.h"
#include "SimpleShooterCharacter.h"
#include "GameFramework/Controller.h"

void ASimpleShooterPlayerController::BeginPlayingState()
{
	Super::BeginPlayingState();
	AddState();
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Try AddState!"));
}


void ASimpleShooterPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	GameModeRef = (ASimpleShooterGameMode*)GetWorld()->GetAuthGameMode();

	if (GameModeRef)
		GameModeRef->AddNewController(this);

	if (PlayerScoreClass && PlayerWidgetRef)
	{
		PlayerScore = CreateWidget<UPlayerScoreUIWidget>(PlayerWidgetRef, PlayerScoreClass);
		PlayerWidgetRef->AddNewScoreWidget(PlayerScore);
	}
} 

void ASimpleShooterPlayerController::SetHPWidget(float NewHP)
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

void ASimpleShooterPlayerController::SetHidenScoreWidget(bool NewHiden)
{
	PlayerWidgetRef->ShowScoreBlock(NewHiden);
}

void ASimpleShooterPlayerController::AddState()
{
	if (!bStateIsAdd)
	{
		PlayerStateRef = GetPlayerState<ASimpleShooterPS>();
		if (!PlayerStateRef)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("AddState error!"));
			FTimerHandle Timer;
			GetWorldTimerManager().SetTimer(Timer, this, &ASimpleShooterPlayerController::AddState, 1, true); //NotPerfect solution, try addstate after 1 second and repid if fail
			return;
		}
		bStateIsAdd = true;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("AddState!"));

		GetWorldTimerManager().SetTimer(ScoreUpdateTimer, this, &ASimpleShooterPlayerController::UpdateScore, 0.5f, true);
	}
}

void ASimpleShooterPlayerController::UpdateScore_Implementation()
{
	if (PlayerScore && PlayerStateRef)
	{
		PlayerScore->UpdateScoreInWidget(PlayerStateRef->GetPlayerId(), PlayerStateRef->PlayerScore, PlayerStateRef->PlayerDeaths, PlayerStateRef->PlayerHeadShots);
	}
}
