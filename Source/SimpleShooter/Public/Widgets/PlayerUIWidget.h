// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUIWidget.generated.h"


class UPlayerScoreUIWidget;
/**
 * 
 */
UCLASS()
class SIMPLESHOOTER_API UPlayerUIWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* HPBlock = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UVerticalBox* VBoxScore = nullptr;

	UPROPERTY()
	TArray<UPlayerScoreUIWidget*> ScoreWidgetsArray;

	void SetHPBlock(float NewHP);
	void SetNewHidenHPBlock(bool NewHiden);

	void ShowScoreBlock(bool NewHiden);

	void AddNewScoreWidget(UPlayerScoreUIWidget* NewScoreWidget);
	void RemoveScoreWidget(UPlayerScoreUIWidget* ScoreWidgetToRemove);
};
