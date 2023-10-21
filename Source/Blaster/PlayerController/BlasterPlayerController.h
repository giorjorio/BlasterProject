#pragma once

#include "CoreMinimal.h"

#include "Blaster/Weapons/WeaponTypes.h"
#include "GameFramework/PlayerController.h"

#include "BlasterPlayerController.generated.h"


class ABlasterGameMode;
class ABlasterHUD;
class UCharacterOverlay;
class UInputAction;
class UInputComponent;
class UReturnToMainMenu;
class UUserWidget;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);

UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	/*
	* ElimAnnouncement
	*/
	void BroadcastElim(APlayerState* Attacker, APlayerState* Victim);

	/*
	* Match countdown
	*/
	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();
	void HandleCountdown();
	
	/*
	* Setting and Updating the HUD
	*/
	void SetBlasterHUD();
	void SetHUDAnnouncementCountdown(float CountdownTime);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDEquippedWeaponIcon(float Opacity, bool IsSecondary = false, UTexture2D* WeaponIcon = nullptr);
	void SetHUDGrenades(int32 Grenades);
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDShield(float Shield, float MaxShield); void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDRoundProgressBars();
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void UpdateSpeedBuffBar(float Timeleft, float BuffTime);
	void UpdateJumpBuffBar(float TimeLeft, float BuffTime);

	/*
	* Sync the time
	*/
	float SingleTripTime = 0.f;


	virtual float GetServerTime(); // Synced with server world clock
	virtual void ReceivedPlayer() override; // Sync with server clock as soon as possible

	/*
	* Ping warning
	*/
	FHighPingDelegate HighPingDelegate;


protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
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
	float ClientServerDelta = 0.f; // Difference between client and server time

	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.f; // How often client should sync up with server
	
	float TimeSyncRunningTime = 0.f; // How much time passed since the last sync

	void CheckTimeSync(float DeltaTime); // Checking how much time passed since the last sync

	// Requests the current server time, passing in the client's time when the request was sent
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	// Reports the current server time to the client in response to ServerRequestServerTime
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidGame(FName StateOfMatch, float Warmup, float Match, float StartingTime, float Cooldown);

	/*
	* ElimAnnouncement
	*/
	UFUNCTION(Client, Reliable)
	void ClientElimAnnouncement(APlayerState* Attacker, APlayerState* Victim);

	/*
	* HUD
	*/
	void ShowReturnToMainMenu();

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

	UFUNCTION(Server, Reliable)
	void ServerReportPingStatus(bool bHighPing);


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

	/*
	* Return to main menu
	*/
	UPROPERTY(EditAnywhere, Category = HUD)
	TSubclassOf<UUserWidget> ReturnToMainMenuWidget;

	UPROPERTY()
	UReturnToMainMenu* ReturnToMainMenu;

	bool bReturnToMainMenuOpen = false;

public:
	FORCEINLINE bool GetDisabledGameplay() const { return bDisableGameplay; }


private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EscapeAction;

};
