// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoundProgressBarWidget.generated.h"

class UCanvasPanelSlot;
class UImage;
class UTextBlock;
class UTexture2D;

UCLASS()
class BLASTER_API URoundProgressBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	bool EnableSegments;

	UPROPERTY(EditAnywhere)
	float BarSize;

	UPROPERTY(EditAnywhere)
	float InStrokeSize;

	UPROPERTY(EditAnywhere)
	float OutStrokeSize;

	UPROPERTY(EditAnywhere)
	float IconX;

	UPROPERTY(EditAnywhere)
	float IconY;

	UPROPERTY(EditAnywhere)
	UMaterialInstance* RoundProgressBar_MI;

	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* RoundProgressBar_Dynamic_MI;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StatText;

	UPROPERTY(EditAnywhere)
	UTexture2D* IconPicture;

	UPROPERTY(meta = (BindWidget))
	UImage* RoundBar;

	UPROPERTY(meta = (BindWidget))
	UImage* Icon;

	UCanvasPanelSlot* Slot1;




	void SetProgressBar(bool bShow = true);

	void SetPercentage(float Percent, bool bShow = true);


	
};
