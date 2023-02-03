// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SimpleShooterPlayerController.generated.h"

/**
 * 
 */

class UPlayerUIWidget;

UCLASS()
class SIMPLESHOOTER_API ASimpleShooterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay();

public:

	UPROPERTY(BlueprintReadWrite)
	UPlayerUIWidget* PlayerWidgetRef;

	void SetGPWidget(float NewHP);
	void SetHidenHPWidget(bool NewHiden);

};
