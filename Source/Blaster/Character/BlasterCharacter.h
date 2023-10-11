// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Blaster/BlasterTypes/CombatState.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "InputActionValue.h"

#include "BlasterCharacter.generated.h"


class ABlasterPlayerController;
class ABlasterPlayerState;
class AController;
class AWeapon;
class UAnimMontage;
class UBuffComponent;
class UCameraComponent;
class UCombatComponent;
class UInputAction;
class UInputComponent;
class UInputMappingContext;
class USoundCue;
class USpringArmComponent;
class UWidgetComponent;


UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{

	GENERATED_BODY()

public:
	/*
	* Essentials
	*/
	ABlasterCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

	/*
	* HUD
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);

	void UpdateHUDAmmo();
	void SpawnDefaultWeapon();

	void UpdateHUDHealth(); //Health
	void UpdateHUDShield(); //Shield
	void CreateRoundProgressBars();

	/*
	* Montages
	*/
	void PlayFireMontage(bool bAiming);
	void PlayHitReactMontage();
	void PlayReloadMontage();
	void PlayElimMontage();
	void PlayThrowGrenadeMontage();

	/*
	* Other
	*/
	UPROPERTY(Replicated)
	bool bDisableCharacterGameplay = false;

	virtual void Destroyed() override;
	void Elim();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

	virtual void OnRep_ReplicatedMovement() override;
	
	

protected:
	/*
	* Essentials
	*/
	void AddInputMappinContextToPlayer();
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	//virtual void Restart() override;

	/*
	* For Animations
	*/
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();

	/*
	* Gameplay Mechanics
	*/
	// Poll for any relevant classes and initialize our HUD
	void PollInit();

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

	void RotateInPlace(float DeltaTime);
	

	/*
	* Input Actions
	*/
	void AimButtonPressed();
	void AimButtonReleased();
	void ChoosePrimaryWeaponPressed();
	void ChooseSecondaryWeaponPressed();
	void CrouchButtonPressed();
	void CrouchButtonReleased();
	void Equip();
	void FireButtonPressed();
	void FireButtonReleased();
	virtual void Jump() override;
	void Look(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);
	void ReloadButtonPressed();
	void SwapWeaponsPressed();
	void ThrowGrenadeButtonPressed();

private:
	/*
	* Actions
	*/
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	UFUNCTION(Server, Reliable)
	void ServerSwapWeaponsPressed();

	/*
	* AimOffset
	*/
	bool bRotateRootBone;
	ETurningInPlace TurningInPlace;
	float AO_Pitch;
	float AO_Yaw;
	float AO_YawInterp;
	float CalculateSpeed();
	float ProxyYaw;
	float TimeSinceLastMovementReplication;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float TurnSpeed;

	float TurnThreshold = 0.5f;
	FRotator ProxyRotation;
	FRotator ProxyRotationLastFrame;
	FRotator StartingAimRotation;

	void TurnInPlace(float DeltaTime);

	/*
	* Animation Montages
	*/
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ElimMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ThrowGrenadeMontage;

	/*
	* Camera
	*/
	UPROPERTY(EditAnywhere)
	float CameraThreshold = 120.f;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* CameraBoom;

	void HideCameraIfCharacterClose();
	void HideCharacterMesh(bool bHide);

	/*
	* Default Weapon
	*/
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;

	/*
	* Dissolve Effect
	*/
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;

	// Material instance set on the Blueprint, used with the dynamic material instance
	UPROPERTY(EditAnywhere, Category = "Elim")
	UMaterialInstance* DissolveMaterialInstance;

	//Dynamic Instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, Category = "Elim")
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	void StartDissolve();

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);

	/*
	* ElimBot
	*/
	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent;

	UPROPERTY(EditAnywhere)
	USoundCue* ElimBotSound;

	/*
	* Elimination
	*/
	bool bElimmed = false;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;

	FTimerHandle ElimTimer;
	void ElimTimerFinished();

	/*
	* Grenade
	*/
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* AttachedGrenade;

	/*
	* Health
	*/
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health(float LastHealth);

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;

	/*
	* Project Components or Classes
	*/
	UPROPERTY()
	ABlasterPlayerController* BlasterPlayerController;

	UPROPERTY()
	ABlasterPlayerState* BlasterPlayerState;

	UPROPERTY(VisibleAnywhere)
	UBuffComponent* Buff;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* Combat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* OverheadWidget;

	/*
	* Shield
	*/
	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere, Category = "Player Stats")
	float Shield = 0.f;

	UFUNCTION()
	void OnRep_Shield(float LastShield);

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxShield = 100.f;

	/*
	* Weapon
	*/
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

public:

	/*
	* Getters
	*/
	bool bIsWeaponEquipped();
	bool bIsAiming();
	FORCEINLINE bool GetDisableCharacterGameplay() const { return bDisableCharacterGameplay; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE bool IsHPFull() const { return Health == MaxHealth; }
	FORCEINLINE bool IsShieldFull() const { return Shield == MaxShield; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	FORCEINLINE ABlasterPlayerController* GetBlasterPlayerController() const { return BlasterPlayerController; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE UAnimMontage* GetReloadMontage() const { return ReloadMontage; }
	FORCEINLINE UBuffComponent* GetBuff() const { return Buff; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
	AWeapon* GetEquippedWeapon();
	ECombatState GetCombatState() const;
	FVector GetHitTarget() const;


	/*
	* Setters
	*/
	FORCEINLINE void SetHealth(float HealthAmount) { Health = HealthAmount; }
	FORCEINLINE void SetShield(float ShieldAmount) { Shield = ShieldAmount; }
	void SetOverlappingWeapon(AWeapon* Weapon);
	void SetDisableCharacterGameplay(bool DisableGameplay);

private:

	/*
	* INPUT
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* BlasterMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchHoldAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchPressedAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapWeaponsAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ThrowGrenadeAction;

};
