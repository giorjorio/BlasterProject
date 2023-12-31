#pragma once

#include "CoreMinimal.h"

#include "Blaster/BlasterTypes/CombatState.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/Weapons/WeaponTypes.h"
#include "Components/ActorComponent.h"


#include "CombatComponent.generated.h"

#define TRACE_LENGTH 80000.f

class AFlag;
class ABlasterCharacter;
class ABlasterPlayerController;
class ABlasterHUD;
class AWeapon;
class AProjectile;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/*
	* Essentials
	*/
	friend ABlasterCharacter;
	UCombatComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	* Equipping Weapon
	*/
	void EquipWeapon(AWeapon* WeaponToEquip);

	/*
	* Firing Weapon
	*/
	void FireButtonPressed(bool bPressed);

	/*
	* For Animations
	*/
	void JumpToShotgunEnd();

	/*
	* HUD
	*/
	void UpdateCarriedAmmo();
	void UpdatePrimaryWeaponIcon(float Opacity = 1.f);
	void UpdateSecondaryWeaponIcon(float Opacity = 0.5f);
	void UpdateWeaponAmmo();

	/*
	* Picking up ammo
	*/
	bool PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);

	/*
	* Reloading
	*/
	bool bLocallyReloading = false;

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	UFUNCTION(BlueprintCallable)
	void FinishSwap();

	UFUNCTION(BlueprintCallable)
	void FinishSwapAttachWeapons();

	void Reload();

	UFUNCTION(BlueprintCallable)
	void ShotgunShellReload();

	/*
	* Throw Grenade
	*/
	UPROPERTY(EditAnywhere)
	float SpawnAdjustment = 40.f;

	UFUNCTION(BlueprintCallable)
	void ThrowGrenadeFinished();

	UFUNCTION(BlueprintCallable)
	void LaunchGrenade();

	UFUNCTION(Server, Reliable)
	void ServerLaunchGrenade(const FVector_NetQuantize& Target);


protected:
	/*
	* Essentials
	*/
	virtual void BeginPlay() override;

	/*
	* Aiming
	*/
	void SetAiming(bool bIsAiming);
	
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	void SetHUDCrosshairs(float DeltaTime);
	
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	/*
	* Firing Weapon
	*/
	void Fire();
	void FireProjectileWeapon();
	void FireHitScanWeapon();
	void FireShotgun();
	void LocalFire(const FVector_NetQuantize& TraceHitTarget);
	void ShotgunLocalFire(const TArray<FVector_NetQuantize>& TraceHitTargets);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget, float FireDelay);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets);

	/*
	* Equip Weapon
	*/
	void AttachActorToBackpack(AActor* ActorToAttach);
	void AttachActorToLeftHand(AActor* ActorToAttach);
	void AttachActorToRightHand(AActor* ActorToAttach);
	void DropEquippedWeapon();
	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
	void OnRep_SecondaryWeapon();

	void PlayEquipWeaponSound(AWeapon* WeaponToEquip);
	void ReloadEmptyWeapon();
	void SwapWeapons();

	/*
	* Flag
	*/
	void AttachFlagToLeftHand(AWeapon* Flag);
	
	/*
	* Reloading
	*/
	int32 AmountToReload();

	void HandleReload();

	UFUNCTION(Server, Reliable)
	void ServerReload();

	/*
	* Throwing Grenade
	*/
	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> GrenadeClass;

	void ThrowGrenade();

	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();

	void ShowAttachedGrenade(bool bShowGrenade);

private:
	/*
	* Project Component or Classes
	*/
	UPROPERTY()
	ABlasterCharacter* Character;

	UPROPERTY()
	ABlasterPlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;
	
	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
	AWeapon* SecondaryWeapon;

	UPROPERTY()
	ABlasterHUD* HUD;

	/*
	* Aiming and FOV
	*/
	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bAiming = false;

	UFUNCTION()
	void OnRep_Aiming();

	bool bAimButtonPressed = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float AimWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float BaseWalkSpeed;

	//FOV when not aiming; set to the camera's base FOV in BeginPlay
	float DefaultFOV;

	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;

	void InterptFOV(float DeltaTime);

	/*
	* Ammo
	*/
	//Carried ammo for the currently-equipped weapon
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere)
	TMap<EWeaponType, int32> MaxCarriedAmmoMap;

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 60;

	UPROPERTY(EditAnywhere)
	int32 StartingGrenadeLauncherAmmo = 0;

	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo = 24;

	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo = 0;

	UPROPERTY(EditAnywhere)
	int32 StartingShotgunAmmo = 0;

	UPROPERTY(EditAnywhere)
	int32 StartingSMGAmmo = 0;

	UPROPERTY(EditAnywhere)
	int32 StartingSniperAmmo = 0;

	UPROPERTY(EditAnywhere)
	int32 MaxARAmmo = 300;

	UPROPERTY(EditAnywhere)
	int32 MaxGrenadeLauncherAmmo = 30;

	UPROPERTY(EditAnywhere)
	int32 MaxPistolAmmo = 120;

	UPROPERTY(EditAnywhere)
	int32 MaxRocketAmmo = 10;

	UPROPERTY(EditAnywhere)
	int32 MaxShotgunAmmo = 40;

	UPROPERTY(EditAnywhere)
	int32 MaxSMGAmmo = 200;

	UPROPERTY(EditAnywhere)
	int32 MaxSniperAmmo = 30;

	void InitializeCarriedAmmo();
	void UpdateAmmoValues();
	void UpdateShotgunAmmoValues();

	/*
	* Combat State
	*/
	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	/*
	* Fire & Automatic Fire
	*/
	bool bCanFire = true;
	bool bFireButtonPressed;
	FTimerHandle FireTimer;
	bool CanFire();

	void StartFireTimer();
	void FireTimerFinished();

	/*
	* Flag
	*/
	UPROPERTY(ReplicatedUsing = OnRep_TheFlag)
	AFlag* TheFlag = nullptr;

	UFUNCTION()
	void OnRep_TheFlag();

	UPROPERTY(ReplicatedUsing = OnRep_HoldingTheFlag)
	bool bHoldingTheFlag = false;

	UFUNCTION()
	void OnRep_HoldingTheFlag();

	/*
	* Grenades
	*/
	UPROPERTY(ReplicatedUsing = OnRep_Grenades)
	int32 Grenades;

	UPROPERTY(EditAnywhere)
	int32 StartingGrenades = 2;

	UFUNCTION()
	void OnRep_Grenades();

	void UpdateGrenades();

	/*
	* HUD and Crosshairs
	*/
	UPROPERTY(EditAnywhere)
	float BaselineSpread;

	UPROPERTY(EditAnywhere)
	float CrosshairAimingSpreadScale;

	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;

	UPROPERTY(EditAnywhere)
	float ShootingSpread;

	UPROPERTY(EditAnywhere)
	float ShootingSpreadInterpSpeed;

	FVector HitTarget;

	FHUDPackage HUDPackage;

	

public:
	FORCEINLINE int32 GetGrenades() const { return Grenades; }
	bool ShouldSwapWeapons();
};
