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
	//if (ABlasterPlayerState* PlayerState = Cast<ABlasterPlayerState>(InPawn->GetPlayerState<APlayerState>()))
	//{
	//	FString PlayerName = PlayerState->GetPlayerName();
	//	SetDisplayText(PlayerName);
	//}
	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;
	switch (RemoteRole)
	{
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		UE_LOG(LogTemp, Warning, TEXT("UOverheadWidget::ROLE_Authority"));
		break;
	case ENetRole::ROLE_AutonomousProxy:
		UE_LOG(LogTemp, Warning, TEXT("UOverheadWidget::ROLE_AutonomousProxy"));
		Role = FString("Autonomous Proxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		UE_LOG(LogTemp, Warning, TEXT("UOverheadWidget::ROLE_SimulatedProxy"));
		Role = FString("Simulated Proxy");
		break;
	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	}
	FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *Role);
	SetDisplayText(RemoteRoleString);
}

void UOverheadWidget::NativeDestruct()
{

	Super::NativeDestruct();
}
