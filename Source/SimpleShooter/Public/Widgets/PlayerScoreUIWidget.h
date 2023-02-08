// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerScoreUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class SIMPLESHOOTER_API UPlayerScoreUIWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Player_ID;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Kills;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Deaths;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* HeadShots;

	void UpdateScoreInWidget(int NewID, int NewKills, int NewDeath, int NewHeadshots);
	void SetTextInBlock(UTextBlock* TextBlock, int NewValue);
};
