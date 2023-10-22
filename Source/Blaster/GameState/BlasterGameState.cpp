// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameState.h"

#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Net/UnrealNetwork.h"

void ABlasterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ABlasterGameState, TopScoringPlayers);
	DOREPLIFETIME(ABlasterGameState, RedTeamScore);
	DOREPLIFETIME(ABlasterGameState, BlueTeamScore);
}

void ABlasterGameState::UpdateTopScore(ABlasterPlayerState* ScoringPlayer)
{
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
	else if(TopScore == ScoringPlayer->GetScore())
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (TopScore < ScoringPlayer->GetScore())
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
}

/*
* Teams
*/
void ABlasterGameState::OnRep_RedTeamScore()
{

}

void ABlasterGameState::OnRep_BlueTeamScore()
{

}

void ABlasterGameState::RedTeamScores()
{
	++RedTeamScore;
}

void ABlasterGameState::BlueTeamScores()
{
	++BlueTeamScore;
}
