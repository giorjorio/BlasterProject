// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RoundProgressBarWidget.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"


class UImage;
class UProgressBar;
class UTextBlock;
class UWidgetAnimation;


UCLASS()
class BLASTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

protected:
	//virtual void BeginPlay() override;
public:

	/*
	* Ammo
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CarriedAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GrenadesText;

	UPROPERTY(meta = (BindWidget))
	UImage* PrimaryWeaponImage;

	UPROPERTY(meta = (BindWidget))
	UImage* SecondaryWeaponImage;

	/*
	* Health
	*/
	UPROPERTY(meta = (BindWidget))
	URoundProgressBarWidget* RoundHealthBar;

	/*
	* Jump Buff
	*/
	UPROPERTY(meta = (BindWidget))
	URoundProgressBarWidget* RoundJumpBuffBar;

	/*
	* Match info
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MatchCountdownText;

	/*
	* Player's connection
	*/
	UPROPERTY(meta = (BindWidget))
	UImage* HighPingImage;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* HighPingAnimation;
	
	/*
	* Score
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DefeatsAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreAmount;

	/*
	* Shield
	*/
	UPROPERTY(meta = (BindWidget))
	URoundProgressBarWidget* RoundShieldBar;

	/*
	* Speed Buff
	*/
	UPROPERTY(meta = (BindWidget))
	URoundProgressBarWidget* RoundSpeedBuffBar;

};
