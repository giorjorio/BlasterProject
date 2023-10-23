// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Blaster/BlasterTypes/CombatState.h"
#include "Blaster/BlasterTypes/Team.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "InputActionValue.h"

#include "BlasterCharacter.generated.h"

class ABlasterGameMode;
class ABlasterPlayerController;
class ABlasterPlayerState;
class AController;
class AWeapon;
class UAnimMontage;
class UBoxComponent;
class UBuffComponent;
class UCameraComponent;
class UCombatComponent;
class UInputAction;
class UInputComponent;
class UInputMappingContext;
class ULagCompensationComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class USoundCue;
class USpringArmComponent;
class UWidgetComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);

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
	virtual void Destroyed() override;

	/*
	* Elim
	*/
	void Elim(bool bPlayerLeftGame);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim(bool bPlayerLeftGame);

	/*
	* Hit boxes used for server-side rewind
	*/
	UPROPERTY()
	TMap<FName, UBoxComponent*> HitCollisionBoxes;

	/*
	* HUD
	*/
	void CreateRoundProgressBars();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);
	
	void SpawnDefaultWeapon();
	void UpdateHUDAmmo();
	void UpdateHUDHealth(); //Health
	void UpdateHUDShield(); //Shield

	/*
	* Leadership
	*/
	UFUNCTION(NetMulticast, Reliable)
	void MulticastGainedTheLead();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLostTheLead();

	/*
	* Leaving the game
	*/
	bool bLeftGame = false;

	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();

	FOnLeftGame OnLeftGame;

	/*
	* Montages
	*/
	void PlayElimMontage();
	void PlayFireMontage(bool bAiming);
	void PlayHitReactMontage();
	void PlayReloadMontage();
	void PlaySwapMontage();
	void PlayThrowGrenadeMontage();

	/*
	* Movement
	*/
	UPROPERTY(Replicated)
	bool bDisableCharacterGameplay = false;

	virtual void OnRep_ReplicatedMovement() override;
	
	/*
	* Swap weapons
	*/
	bool bFinishedSwapping = false;

	/*
	* Teams
	*/
	void SetTeamColor(ETeam Team);

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
	* Hit boxes used for server-side rewind
	*/
	UPROPERTY(EditAnywhere)
	UBoxComponent* HeadBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* PelvisBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Spine02Box;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Spine03Box;

	UPROPERTY(EditAnywhere)
	UBoxComponent* UpperArmLeftBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* UpperArmRightBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* LowerArmLeftBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* LowerArmRightBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* HandLeftBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* HandRightBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* BackpackBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* BlanketBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* ThighLeftBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* ThighRightBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* CalfLeftBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* CalfRightBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* FootLeftBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* FootRightBox;
	
	/*
	* Weapon
	*/
	void DropOrDestroyWeapon(AWeapon* Weapon);
	void DropOrDestroyWeapons();

	/*
	* Input Actions
	*/
	void AimButtonPressed();
	void AimButtonReleased();
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
	UAnimMontage* SwapMontage;

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

	void HideCharacterIfCameraClose();
	void HideCharacterMesh(bool bHide);

	/*
	* Components or Classes
	*/
	UPROPERTY()
	ABlasterGameMode* BlasterGameMode;

	UPROPERTY()
	ABlasterPlayerController* BlasterPlayerController;

	UPROPERTY()
	ABlasterPlayerState* BlasterPlayerState;

	UPROPERTY(VisibleAnywhere)
	UBuffComponent* Buff;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* Combat;

	UPROPERTY(VisibleAnywhere)
	ULagCompensationComponent* LagCompensation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* OverheadWidget;

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
	UPROPERTY(VisibleAnywhere, Category = "Elim")
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
	* Leadership effects
	*/
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* CrownSystem;

	UPROPERTY()
	UNiagaraComponent* CrownComponent;

	

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
	* Team colors
	*/
	UPROPERTY(EditAnywhere)
	UMaterialInstance* OriginalMaterial;

	UPROPERTY(EditAnywhere, Category = "Elim")
	UMaterialInstance* BlueDissolveMatInst;
	
	UPROPERTY(EditAnywhere, Category = Team)
	UMaterialInstance* BlueMaterial;

	UPROPERTY(EditAnywhere, Category = "Elim")
	UMaterialInstance* RedDissolveMatInst;

	UPROPERTY(EditAnywhere, Category = Team)
	UMaterialInstance* RedMaterial;

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
	bool IsWeaponEquipped();
	bool IsAiming();
	bool IsLocallyReloading();
	FORCEINLINE bool GetDisableCharacterGameplay() const { return bDisableCharacterGameplay; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE bool IsHoldingTheFlag() const;
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
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensation; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
	AWeapon* GetEquippedWeapon();
	ECombatState GetCombatState() const;
	ETeam GetTeam();
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
