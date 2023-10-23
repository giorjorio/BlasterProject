// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "GameFramework/GameStateBase.h"
#include "MultiplayerSessionsSubsystem.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

	UGameInstance* GameInstanse = GetGameInstance();
	if (GameInstanse)
	{
		UMultiplayerSessionsSubsystem* Subsystem = GameInstanse->GetSubsystem<UMultiplayerSessionsSubsystem>();
		check(Subsystem);

		if (NumberOfPlayers == Subsystem->DesiredNumPublicConnections)
		{
			if (UWorld* World = GetWorld())
			{
				bUseSeamlessTravel = true;

				FString MatchType = Subsystem->DesiredMatchType;
				if(MatchType == "DeathMatch")
				{
					World->ServerTravel(FString("/Game/Maps/BlasterMap?listen"));
				}
				else if (MatchType == "TeamDeathMatch")
				{
					World->ServerTravel(FString("/Game/Maps/Teams?listen"));
				}
				else if (MatchType == "CaptureTheFlag")
				{
					World->ServerTravel(FString("/Game/Maps/CaptureTheFlag?listen"));
				}
			}
		}
	}


	
	
}

//void ALobbyGameMode::Tick(float DeltaTime)
//{
//	if (GetWorld()->GetTimeSeconds() >= 10.f)
//	{
//		if (UWorld* World = GetWorld())
//		{
//			bUseSeamlessTravel = true;
//			World->ServerTravel(FString("/Game/Maps/BlasterMap?listen"));
//		}
//	}
//}
