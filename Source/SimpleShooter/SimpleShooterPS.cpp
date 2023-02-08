// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleShooterPS.h"
#include "SimpleShooterGameMode.h"
#include "Net/UnrealNetwork.h"

void ASimpleShooterPS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASimpleShooterPS, PlayerScore);
	DOREPLIFETIME(ASimpleShooterPS, PlayerDeaths);
	DOREPLIFETIME(ASimpleShooterPS, PlayerHeadShots);
}