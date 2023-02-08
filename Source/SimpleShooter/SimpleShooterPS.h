// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SimpleShooterPS.generated.h"


class ASimpleShooterGameMode;
/**
 * 
 */
UCLASS()
class SIMPLESHOOTER_API ASimpleShooterPS : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated)
	int PlayerScore = 0;

	UPROPERTY(Replicated)
	int PlayerDeaths = 0;

	UPROPERTY(Replicated)
	int PlayerHeadShots = 0;

};
