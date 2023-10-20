// Fill out your copyright notice in the Description page of Project Settings.


#include "HandGrenade.h"

#include "Blaster/Blaster.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AHandGrenade::AHandGrenade()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grenade Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Ignore);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->bShouldBounce = true;
	
}

void AHandGrenade::BeginPlay()
{
	AActor::BeginPlay();

	SpawnTrailSystem();
	StartDestroyTimer();
	//StartCollisionDelayTimer();


	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AHandGrenade::OnBounce);

}

void AHandGrenade::StartCollisionDelayTimer()
{
	GetWorldTimerManager().SetTimer(
		CollisionTimer,
		this,
		&AHandGrenade::CollisionTimerFinished,
		NoCollisionTime
	);
}

void AHandGrenade::CollisionTimerFinished()
{
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Block);
}

void AHandGrenade::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (BounceSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			BounceSound,
			GetActorLocation()
		);
	}
}

void AHandGrenade::Destroyed()
{
	ExplodeDamage(BaseDamage, MinimumDamage, DamageInnerRadius, DamageOuterRadius, DamageFalloff);

	Super::Destroyed();
}