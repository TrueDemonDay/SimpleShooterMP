// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class SIMPLESHOOTER_API UPlayerUIWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* HPBlock;

	void SetHPBlock(float NewHP);
	void SetNewHidenHPBlock(bool NewHiden);

};
