// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"

#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"


void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerName(APawn* InPawn)
{
	if (ABlasterPlayerState* PlayerState = Cast<ABlasterPlayerState>(InPawn->GetPlayerState<APlayerState>()))
	{
		FString PlayerName = PlayerState->GetPlayerName();
		SetDisplayText(PlayerName);
	}
	//FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *Role);
	//SetDisplayText(RemoteRoleString);
}

void UOverheadWidget::NativeDestruct()
{

	Super::NativeDestruct();
}
