// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"


class UImage;
class UProgressBar;
class UTextBlock;


UCLASS()
class BLASTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	/*
	* Score
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DefeatsAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreAmount;
	
	/*
	* Health
	*/
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	/*
	* Match info
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MatchCountdownText;
	
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
	UImage* WeaponImage;



};
