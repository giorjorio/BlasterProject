// Fill out your copyright notice in the Description page of Project Settings.


#include "RoundProgressBarWidget.h"

#include "Components/Image.h"


void URoundProgressBarWidget::SetProgressBar(bool bShow)
{
	RoundProgressBar_Dynamic_MI = UMaterialInstanceDynamic::Create(RoundProgressBar_MI, this);
	RoundBar->SetBrushFromMaterial(RoundProgressBar_Dynamic_MI);
	if(!bShow)
	{
		RoundBar->SetVisibility(ESlateVisibility::Hidden);
	}
	
}

void URoundProgressBarWidget::SetPercentage(float Percent, bool bShow)
{
	RoundProgressBar_Dynamic_MI->SetScalarParameterValue(TEXT("Percent"), Percent);
	if (!bShow)
	{
		RoundBar->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		RoundBar->SetVisibility(ESlateVisibility::Visible);
	}
}
