// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WeaponTypes.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

class ABlasterCharacter;
class ABlasterPlayerController;
class ACasing;
class UAnimationAsset;
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
	void SetHUDAmmo();
	void ShowPickupWidget(bool bShowWidget);
	virtual void Fire(const FVector& HitTarget);
	void Dropped();
	void AddAmmo(int32 AmmoToAdd);
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
		 

protected:

	virtual void BeginPlay() override;

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

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category = "WeaponProperties")
	UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACasing> CasingClass;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo)
	int32 Ammo;

	UFUNCTION()
	void OnRep_Ammo();

	void SpendRound();

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	UPROPERTY()
	ABlasterCharacter* BlasterOwnerCharacter;
	UPROPERTY()
	ABlasterPlayerController* BlasterOwnerController;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

public:	

	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	bool IsFull();
	bool IsEmpty();
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }

};
