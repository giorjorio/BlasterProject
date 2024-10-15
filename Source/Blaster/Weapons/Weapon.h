// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Blaster/BlasterTypes/Team.h"
#include "WeaponTypes.h"

#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_EquippedSecondary UMETA(DisplayName = "Equipped Secondary"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EFireType : uint8
{
	EFT_HitScan UMETA(DisplayName = "Hit Scan Weapon"),
	EFT_Projectile UMETA(DisplayName = "Projectile Weapon"),
	EFT_Shotgun UMETA(DisplayName = "Shotgun Weapon"),
	EFT_MAX UMETA(DisplayName = "DefaultMAX")
};

class ABlasterCharacter;
class ABlasterPlayerController;
class ACasing;
class UAnimationAsset;
class UNiagaraComponent;
class UNiagaraSystem;
class USoundCue;
class USphereComponent;
class UTexture2D;
class UWidgetComponent;


UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	

	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	
	/*
	* Weapon functionality
	*/
	void ShowPickupWidget(bool bShowWidget);
	virtual void Fire(const FVector& HitTarget);
	virtual void Dropped();

	/*
	* Ammo
	*/
	void AddAmmo(int32 AmmoToAdd);
	void SetHUDAmmo();
	
	UPROPERTY(EditAnywhere)
	EFireType FireType;

	FVector TraceEndWithScatter(const FVector& HitTarget);

	/*
	* Textures for the weapon crosshairs
	*/
	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairsBottom;

	UPROPERTY(EditAnywhere, Category = "Image")
	UTexture2D* WeaponIcon;

	/*
	* Trace end with scatter
	*/
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	bool bUseScatter = false;

	/*
	* Zoomed FOV while aiming
	*/

	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	/*
	* Automatic fire
	*/
	UPROPERTY(EditAnywhere, Category = Combat)
	float FireDelay = 0.1f;
	UPROPERTY(EditAnywhere, Category = Combat)
	bool bAutomatic = true;

	UPROPERTY(EditAnywhere)
	USoundCue* EquipSound;

	/*
	* Enable or disable custom depth
	*/
	void EnableCustomDepth(bool bEnable);
		 
	bool bDestroyWeapon = false;

	/*
	* Server Side Rewind
	*/
	UPROPERTY(Replicated, EditAnywhere)
	bool bUseServerSideRewind = false;

	UPROPERTY(EditAnywhere)
	bool bUseServerSideRewindDefault = false;

protected:
	virtual void BeginPlay() override;

	/*
	* Other classes
	*/
	UPROPERTY()
	ABlasterCharacter* BlasterOwnerCharacter;

	UPROPERTY()
	ABlasterPlayerController* BlasterOwnerController;

	/*
	* Damage
	*/
	UPROPERTY(EditAnywhere)
	float Damage = 15.f;

	UPROPERTY(EditAnywhere)
	float HeadShotDamage = 15.f;

	/*
	* Ping
	*/
	UFUNCTION()
	void OnPingTooHigh(bool bPingTooHigh);

	void Pollinit();

	bool HasSetController = false;

	/*
	* Trace end with scatter
	*/
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float DistanceToSphere = 800.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float SphereRadius = 75.f;

	/*
	* Weapon State 
	*/
	virtual void OnWeaponStateSet();
	virtual void OnInitial();
	virtual void OnEquipped();
	virtual void OnDropped();
	virtual void OnEquippedSecondary();

	UFUNCTION()
	virtual void OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

private:
	/*
	* Main components
	*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USphereComponent* AreaSphere;

	/*
	* Ammo
	*/
	UPROPERTY(EditAnywhere)
	int32 Ammo;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACasing> CasingClass;

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	// The number of unprocessed server requests for Ammo
	//Incremented in SpendRound, decremented in ClienUpdateAmmo
	int32 Sequence = 0;

	UFUNCTION(Client, Reliable)
	void ClientUpdateAmmo(int32 ServerAmmo);
	
	UFUNCTION(Client, Reliable)
	void ClientAddAmmo(int32 AmmoToAdd);

	void SpendRound();

	/*
	* Animation
	*/
	UPROPERTY(EditAnywhere, Category = "WeaponProperties")
	UAnimationAsset* FireAnimation;

	/*
	* Weapon PickupBeam
	*/
	void SpawnPickupBeam();

	FVector CenterLocation;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* PickupBeamSystem;

	UPROPERTY()
	UNiagaraComponent* PickupBeamSystemComponent;

	/*
	* Weapon PickupWidget
	*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UWidgetComponent* PickupWidget;

	/*
	* Weapon State and type
	*/
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere)
	ETeam Team = ETeam::ET_NoTeam;


public:	
	bool IsFull();
	bool IsEmpty();
	FORCEINLINE ETeam GetTeam() const { return Team; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE float GetHeadShotDamage() const { return HeadShotDamage; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FVector GetCenterLocation();
	void SetWeaponState(EWeaponState State);

	

};
