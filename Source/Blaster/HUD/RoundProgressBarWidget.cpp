// Fill out your copyright notice in the Description page of Project Settings.


#include "RoundProgressBarWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstance.h"


void URoundProgressBarWidget::SetProgressBar(bool bShow)
{
	if(RoundProgressBar_MI)
	{
		RoundProgressBar_Dynamic_MI = UMaterialInstanceDynamic::Create(RoundProgressBar_MI, this);
		if(RoundProgressBar_Dynamic_MI)
		{
			if(EnableSegments)
			{
				RoundProgressBar_Dynamic_MI->SetScalarParameterValue(TEXT("Segments"), 1.f);
			}
			else
			{
				RoundProgressBar_Dynamic_MI->SetScalarParameterValue(TEXT("Segments"), 0.f);
			}

			RoundProgressBar_Dynamic_MI->SetScalarParameterValue(TEXT("BarSize"), BarSize);
			RoundProgressBar_Dynamic_MI->SetScalarParameterValue(TEXT("InStrokeSize"), InStrokeSize);
			RoundProgressBar_Dynamic_MI->SetScalarParameterValue(TEXT("OutStrokeSize"), OutStrokeSize);

			RoundBar->SetBrushFromMaterial(RoundProgressBar_Dynamic_MI);
			Icon->SetBrushFromTexture(IconPicture);
			Icon->SetDesiredSizeOverride(FVector2D(IconX, IconY));

			
			Slot1 = Cast<UCanvasPanelSlot>(Icon->Slot);
			Slot1->SetSize(FVector2D(IconX, IconY));


			
			if (GetName() == "RoundJumpBuffBar" || GetName() == "RoundSpeedBuffBar")
			{
				StatText->SetVisibility(ESlateVisibility::Hidden);
			}

			if (!bShow)
			{
				RoundBar->SetVisibility(ESlateVisibility::Hidden);
				Icon->SetVisibility(ESlateVisibility::Hidden);
				StatText->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	
}

void URoundProgressBarWidget::SetPercentage(float Percent, bool bShow)
{
	if(RoundProgressBar_Dynamic_MI)
	{
		RoundProgressBar_Dynamic_MI->SetScalarParameterValue(TEXT("Percent"), Percent);
		StatText->SetText(FText::AsNumber(FMath::RoundToInt(Percent * 100)));
		if (!bShow)
		{
			RoundBar->SetVisibility(ESlateVisibility::Hidden);
			Icon->SetVisibility(ESlateVisibility::Hidden);
			StatText->SetVisibility(ESlateVisibility::Hidden);
			
		}
		else
		{
			RoundBar->SetVisibility(ESlateVisibility::Visible);
			Icon->SetVisibility(ESlateVisibility::Visible);
			if (GetName() == "RoundJumpBuffBar" || GetName() == "RoundSpeedBuffBar")
			{
				StatText->SetVisibility(ESlateVisibility::Hidden);
			}
			else
			{
				StatText->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}
