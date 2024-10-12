// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileGrenade.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AProjectileGrenade : public AProjectile
{
	GENERATED_BODY()
	
public:
	AProjectileGrenade();
	virtual void Destroyed() override;


#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& Event) override;
#endif


protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	/*UFUNCTION()
	void OnProjectileStop(const FHitResult& ImpactResult);*/

private:

	UPROPERTY(EditAnywhere)
	USoundCue* BounceSound;

	UPROPERTY(EditAnywhere)
	int32 Bounces = 0;

	UPROPERTY(EditAnywhere)
	int32 LastBounce = 2;

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
};
