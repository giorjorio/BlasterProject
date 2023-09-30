// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoundProgressBarWidget.generated.h"

class UImage;

UCLASS()
class BLASTER_API URoundProgressBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UMaterialInstance* RoundProgressBar_MI;

	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* RoundProgressBar_Dynamic_MI;

	UPROPERTY(meta = (BindWidget))
	UImage* RoundBar;

	void SetProgressBar(bool bShow = true);

	void SetPercentage(float Percent, bool bShow = true);


	
};
