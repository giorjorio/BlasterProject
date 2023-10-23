// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &ThisClass::ExitButtonClicked);
	}
	if (DeathMatchCheckbox)
	{
		DeathMatchCheckbox->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckedDeathMatchCheckbox);
	}
	if (TeamDeathMatchCheckbox)
	{
		TeamDeathMatchCheckbox->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckedTeamDeathMatchCheckbox);
	}
	if (CaptureTheFlagCheckbox)
	{
		CaptureTheFlagCheckbox->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckedCaptureTheFlagCheckbox);
	}
	if (NumberOfPlayersTextBox)
	{
		NumberOfPlayersTextBox->OnTextChanged.AddDynamic(this, &ThisClass::OnTextChangesNumberOfPlayersTextBox);
	}


	return true;
}

void UMenu::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString(TEXT("Session created successfully!"))
			);
		}
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(PathToLobby);
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("Failed to create session!"))
			);
		}
		HostButton->SetIsEnabled(true);
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (MultiplayerSessionsSubsystem == nullptr)
	{
		return;
	}

	for (auto Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if (SettingsValue == MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}
	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("No proper session was found!"))
			);
		}
	}
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Red,
			FString(TEXT("UMenu::OnFindSessions: No proper session was found!"))
		);
	}
	HostButton->SetIsEnabled(true);
	JoinButton->SetIsEnabled(true);
	DeathMatchCheckbox->SetIsEnabled(true);
	TeamDeathMatchCheckbox->SetIsEnabled(true);
	CaptureTheFlagCheckbox->SetIsEnabled(true);
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	JoinButton->SetIsEnabled(false);
	DeathMatchCheckbox->SetIsEnabled(false);
	TeamDeathMatchCheckbox->SetIsEnabled(false);
	CaptureTheFlagCheckbox->SetIsEnabled(false);

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UMenu::JoinButtonClicked()
{
	HostButton->SetIsEnabled(false);
	JoinButton->SetIsEnabled(false);
	DeathMatchCheckbox->SetIsEnabled(false);
	TeamDeathMatchCheckbox->SetIsEnabled(false);
	CaptureTheFlagCheckbox->SetIsEnabled(false);



	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}

void UMenu::ExitButtonClicked()
{
	ExitButton->SetIsEnabled(false);

	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}

void UMenu::OnCheckedDeathMatchCheckbox(bool bIsChecked)
{
	if (TeamDeathMatchCheckbox)
	{
		TeamDeathMatchCheckbox->SetCheckedState(ECheckBoxState::Unchecked);
	}
	if (CaptureTheFlagCheckbox)
	{
		CaptureTheFlagCheckbox->SetCheckedState(ECheckBoxState::Unchecked);
	}

	MatchType = FString(TEXT("DeathMatch"));
}

void UMenu::OnCheckedTeamDeathMatchCheckbox(bool bIsChecked)
{
	if (DeathMatchCheckbox)
	{
		DeathMatchCheckbox->SetCheckedState(ECheckBoxState::Unchecked);
	}
	if (CaptureTheFlagCheckbox)
	{
		CaptureTheFlagCheckbox->SetCheckedState(ECheckBoxState::Unchecked);
	}
	MatchType = FString(TEXT("TeamDeathMatch"));

}

void UMenu::OnCheckedCaptureTheFlagCheckbox(bool bIsChecked)
{
	if (DeathMatchCheckbox)
	{
		DeathMatchCheckbox->SetCheckedState(ECheckBoxState::Unchecked);
	}
	if (TeamDeathMatchCheckbox)
	{
		TeamDeathMatchCheckbox->SetCheckedState(ECheckBoxState::Unchecked);
	}
	MatchType = FString(TEXT("CaptureTheFlag"));
}

void UMenu::OnTextChangesNumberOfPlayersTextBox(const FText& Text)
{
	FString Num = Text.ToString();
	NumPublicConnections = FCString::Atoi(*Num);
	UE_LOG(LogTemp, Warning, TEXT("%d"), NumPublicConnections);
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}
