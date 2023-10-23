// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

class UCheckBox;
class UButton;
class UMultiplayerSessionsSubsystem;
class UEditableTextBox;

UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("DeathMatch")), FString LobbyPath = FString(TEXT("/Game/Maps/Lobby")));

protected:

	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	//
	// Callbacks for the custom delegates on the MultiplayerSessionsSubsystem
	//
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);

	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

private:
	UPROPERTY(meta = (BindWidget))
	UCheckBox* DeathMatchCheckbox;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* TeamDeathMatchCheckbox;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* CaptureTheFlagCheckbox;

	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* NumberOfPlayersTextBox;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	UFUNCTION()
	void ExitButtonClicked();

	UFUNCTION()
	void OnCheckedDeathMatchCheckbox(bool bIsChecked);

	UFUNCTION()
	void OnCheckedTeamDeathMatchCheckbox(bool bIsChecked);

	UFUNCTION()
	void OnCheckedCaptureTheFlagCheckbox(bool bIsChecked);

	UFUNCTION()
	void OnTextChangesNumberOfPlayersTextBox(const FText& Text);

	void MenuTearDown();

	// The subsystem designed to handle all online session functionality
	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 NumPublicConnections{4};

	UPROPERTY(BlueprintReadWrite, meta =(AllowPrivateAccess = "true"))
	FString MatchType{TEXT("DeathMatch")};

	FString PathToLobby{TEXT("")};
};
