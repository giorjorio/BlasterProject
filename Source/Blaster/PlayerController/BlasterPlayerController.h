#pragma once

#include "CoreMinimal.h"

#include "Blaster/Weapons/WeaponTypes.h"
#include "GameFramework/PlayerController.h"

#include "BlasterPlayerController.generated.h"


class ABlasterGameMode;
class ABlasterHUD;
class UCharacterOverlay;



UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDAnnouncementCountdown(float CountdownTime);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDGrenades(int32 Grenades);
	void SetHUDEquippedWeaponIcon(float Opacity,  bool IsSecondary = false, UTexture2D * WeaponIcon = nullptr );
	void SetHUDRoundProgressBars();
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDShield(float Shield, float MaxShield);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);

	void SetBlasterHUD();

	void UpdateSpeedBuffBar(float Timeleft, float BuffTime);
	void UpdateJumpBuffBar(float TimeLeft, float BuffTime);
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float GetServerTime(); // Synced with server world clock
	virtual void ReceivedPlayer() override; // Sync with server clock as soon as possible

	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();
	void HandleCountdown();


	float SingleTripTime = 0.f;

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

protected:

	virtual void BeginPlay() override;
	void SetHUDTime();
	void PollInit();

	/*
	* Callback for the custom delegate on the BlasterGameMode
	*/
	UFUNCTION()
	void OnBlasterGameModeBeginPlay();

	/*
	* Client server sync
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

	/*
	* Ping warning
	*/ 
	UPROPERTY(EditAnywhere)
	float CheckPingFrequency = 20.f;

	UPROPERTY(EditAnywhere)
	float HighPingDuration = 5.f;

	float HighPingRunningTime = 0.f;

	UPROPERTY(EditAnywhere)
	float HighPingThreshold = 50.f;

	UPROPERTY(EditAnywhere)
	float PingAnimationRunningTime = 0.f;
	
	void CheckPing(float DeltaTime);
	void HighPingWarning();
	void StopHighPingWarning();


private:
	UPROPERTY()
	ABlasterHUD* BlasterHUD;

	UPROPERTY()
	ABlasterGameMode* BlasterGameMode;


	void InitializeHUD();

	/*
	* FHUDIcons HUDIcons
	*/
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


	/*
	* Pollinit variables
	*/
	float HUDWeaponAmmo;
	bool bInitializeWeaponAmmo = false;

public:
	FORCEINLINE bool GetDisabledGameplay() const { return bDisableGameplay; }

};
