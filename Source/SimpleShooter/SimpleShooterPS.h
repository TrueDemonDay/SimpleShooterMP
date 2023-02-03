// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SimpleShooterPS.generated.h"

/**
 * 
 */
UCLASS()
class SIMPLESHOOTER_API ASimpleShooterPS : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int PlayerScore = 0;

	UPROPERTY()
	int PlayerDeaths = 0;

	UPROPERTY()
	int PlayerHeadShots = 0;
};
