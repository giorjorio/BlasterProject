#pragma once

#include "CoreMinimal.h"

#include "Blaster/Weapons/WeaponTypes.h"
#include "GameFramework/PlayerController.h"

#include "BlasterPlayerController.generated.h"

class ABlasterGameMode;
class ABlasterHUD;
class UCharacterOverlay;

//USTRUCT(BlueprintType)
//struct FHUDIcons
//{
//	GENERATED_BODY()
//
//public:
//
//	UTexture2D* AssaultRifleIcon;
//	UTexture2D* PistolIcon;
//	UTexture2D* SubmachineGunIcon;
//	UTexture2D* ShotgunIcon;
//	UTexture2D* RocketLauncherIcon;
//	UTexture2D* GrenadeLauncherIcon;
//	UTexture2D* SniperRifleIcon;
//};

UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDAnnouncementCountdown(float CountdownTime);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDEquippedWeaponIcon(UTexture2D* WeaponIcon);
	void SetHUDGrenades(int32 Grenades);
	void SetHUDMatchCoundown(float CountdownTime);
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDWeaponAmmo(int32 Ammo);
	
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float GetServerTime(); // Synced with server world clock
	virtual void ReceivedPlayer() override; // Sync with server clock as soon as possible

	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();
	void HandleCountdown();

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

protected:

	virtual void BeginPlay() override;
	void SetHUDTime();
	//void PollInit();

	/*
	* Callback for the custom delegate on the BlasterGameMode
	*/
	UFUNCTION()
	void OnBlasterGameModeBeginPlay();


	/*
	* Sync time between client and server
	*/

	// Requests the current server time, passing in the client's time when the request was sent
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	// Reports the current server time to the client in response to ServerRequestServerTime
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta = 0.f; // Difference between client and server time

	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.f; // How often client should sync up with server

	float TimeSyncRunningTime = 0.f; // How much time passed since the last sync
	void CheckTimeSync(float DeltaTime); // Checking how much time passed since the last sync

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidGame(FName StateOfMatch, float Warmup, float Match, float StartingTime, float Cooldown);

private:
	UPROPERTY()
	ABlasterHUD* BlasterHUD;

	UPROPERTY()
	ABlasterGameMode* BlasterGameMode;

	//FHUDIcons HUDIcons;

	float LevelStartingTime = 0.f;
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float CooldownTime = 0.f;

	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	UCharacterOverlay* CharacterOverlay;




	/*bool bInitializeCharacterOverlay = false;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDefeats;*/
public:
	FORCEINLINE bool GetDisabledGameplay() const { return bDisableGameplay; }

};
