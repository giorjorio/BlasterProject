#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UBoxComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UProjectileMovementComponent;
class UParticleSystem;
class UParticleSystemComponent;
class USoundCue;

UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

	/*
	* Used with Server Side Rewind
	*/
	bool bUseServerSideRewind = false;
	FVector_NetQuantize TraceStart;
	FVector_NetQuantize100 InitialVelocity;

	UPROPERTY(EditAnywhere)
	float InitialSpeed = 0;

	

protected:
	virtual void BeginPlay() override;
	void StartDestroyTimer();
	void DestroyTimerFinished();
	void SpawnTrailSystem();
	void ExplodeDamage(float BaseDamage, float MinimumDamage, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff);

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	float Damage = 20.f;

	UPROPERTY(EditAnywhere)
	float HeadShotDamage = 60.f;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere)
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* TrailSystem;

	UPROPERTY()
	UNiagaraComponent* TrailSystemComponent;

private:

	UPROPERTY(EditAnywhere)
	UParticleSystem* Tracer;

	UPROPERTY()
	UParticleSystemComponent* TracerComponent;

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
	float DestroyTime = 3.f;

public:	
	void SetDamage(float NewDamage);
	void SetHeadShotDamage(float NewDamage);

};
