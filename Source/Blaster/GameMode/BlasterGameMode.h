// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

class ABlasterCharacter;
class ABlasterPlayerController;
class ABlasterPlayerState;
class ACharacter;

// Custom delegate to inform when BlasterGameMode's BeginPlay() happened
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBlasterGameModeBeginPlay);


namespace MatchState
{
	extern BLASTER_API const FName Cooldown; // Match duration has been reached. Display winner and begin cooldown timer
}

UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABlasterGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);
	void PlayerLeftGame(ABlasterPlayerState* PlayerLeaving);
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);
	virtual void Logout(AController* Exiting) override;

	bool bTeamsMatch = false;

	//
	// Custom delegate for the BlasterGameMode class
	//
	FOnBlasterGameModeBeginPlay OnBlasterGameModeBeginPlay;
	bool bHasBeenBeginPlay = false;

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 12.f;

	float LevelStartingTime = 0.f;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:

	float CountdownTime = 0.f;

public:
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
	FORCEINLINE float GetLevelStartingTime() const { return LevelStartingTime; }
};
