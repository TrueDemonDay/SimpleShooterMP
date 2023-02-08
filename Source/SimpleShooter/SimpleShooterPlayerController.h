// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SimpleShooterPlayerController.generated.h"

/**
 * 
 */

class ASimpleShooterPS;
class ASimpleShooterGameMode;
class UPlayerUIWidget;
class UPlayerScoreUIWidget;

UCLASS()
class SIMPLESHOOTER_API ASimpleShooterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay();
	virtual void BeginPlayingState();

public:

	UPROPERTY(EditDefaultsOnly, Category = Widgets)
	TSubclassOf<UPlayerScoreUIWidget> PlayerScoreClass;

	UPROPERTY(BlueprintReadWrite)
	UPlayerUIWidget* PlayerWidgetRef;

	UPROPERTY(BlueprintReadWrite)
	UPlayerScoreUIWidget* PlayerScore;

	UPROPERTY(BlueprintReadWrite)
	ASimpleShooterPS* PlayerStateRef;

	UPROPERTY(BlueprintReadWrite)
	ASimpleShooterGameMode* GameModeRef;

	void SetHPWidget(float NewHP);
	void SetHidenHPWidget(bool NewHiden);
	void SetHidenScoreWidget(bool NewHiden);

	void AddState();

	bool bStateIsAdd = false;

	FTimerHandle ScoreUpdateTimer;

	UFUNCTION(NetMulticast, Reliable)
	void UpdateScore();
};
