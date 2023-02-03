// Copyright Epic Games, Inc. All Rights Reserved.

#include "SimpleShooterGameMode.h"
#include "SimpleShooterHUD.h"
#include "SimpleShooterCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "SimpleShooterPS.h"
#include "SimpleShooterPlayerController.h"


ASimpleShooterGameMode::ASimpleShooterGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ASimpleShooterHUD::StaticClass();
	PlayerStateClass = ASimpleShooterPS::StaticClass();
	PlayerControllerClass = ASimpleShooterPlayerController::StaticClass();
}

FTransform ASimpleShooterGameMode::GetNewRespawLocation()
{
	FTransform NewTransform;
	NewTransform.SetLocation(FVector(0, 0, 900));
	NewTransform.SetScale3D(FVector(1, 1, 1));

	if (RespawnPoints.IsValidIndex(0))
	{
		AActor* RespawnPoint = RespawnPoints[FMath::RandRange(0, RespawnPoints.Num()-1)];
		if (RespawnPoint)
			NewTransform = RespawnPoint->GetActorTransform();
	}

	return NewTransform;
}

void ASimpleShooterGameMode::AddNewRespawnPoint(AActor * NewPoint)
{
	if (NewPoint)
		RespawnPoints.Add(NewPoint);
}
