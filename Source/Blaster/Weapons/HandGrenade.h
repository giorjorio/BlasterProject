// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WeaponTypes.h"

#include "CoreMinimal.h"
#include "Projectile.h"
#include "HandGrenade.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AHandGrenade : public AProjectile
{
	GENERATED_BODY()

public:
	AHandGrenade();
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	void StartCollisionDelayTimer();
	void CollisionTimerFinished();

	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

private:

	UPROPERTY(EditAnywhere)
	USoundCue* BounceSound;

	UPROPERTY(EditAnywhere)
	float BaseDamage = 20.f;

	UPROPERTY(EditAnywhere)
	float MinimumDamage = 10.f;

	UPROPERTY(EditAnywhere)
	float DamageInnerRadius = 200.f;

	UPROPERTY(EditAnywhere)
	float DamageOuterRadius = 500.f;

	UPROPERTY(EditAnywhere)
	float DamageFalloff = 1.f;

	FTimerHandle CollisionTimer;

	float NoCollisionTime = 0.1f;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType = EWeaponType::EWT_Grenade;
};
