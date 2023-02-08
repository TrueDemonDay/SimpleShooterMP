// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SimpleShooterGameMode.generated.h"

class ASimpleShooterPlayerController;
class ASimpleShooterCharacter;

UCLASS(minimalapi)
class ASimpleShooterGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

public:
	ASimpleShooterGameMode();

	UPROPERTY(EditDefaultsOnly, Category = PlayerCharacter)
	TSubclassOf<ASimpleShooterCharacter> PlayerCharacterClass;

	UPROPERTY()
	TArray<AActor*> RespawnPoints;
	
	UPROPERTY()
	TArray<ASimpleShooterPlayerController*> ControllersArray;

	FTransform GetNewRespawLocation();

	void AddNewRespawnPoint(AActor* NewPoint);
	void AddNewController(ASimpleShooterPlayerController* NewController);
};



