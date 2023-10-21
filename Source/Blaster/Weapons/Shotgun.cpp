// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Components/BlasterComponents/LagCompensationComponent.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GamePlayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"


void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) { return; }
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector Start = SocketTransform.GetLocation();

		TMap<ABlasterCharacter*, uint32> HitMap; // maps hit character to number of times hit
		TMap<ABlasterCharacter*, uint32> HeadShotHitMap;

		for (const FVector_NetQuantize& HitTarget : HitTargets)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			if (BlasterCharacter)
			{
				const bool bHeadShot = FireHit.BoneName.ToString() == FString("head");

				if (bHeadShot)
				{
					if (HeadShotHitMap.Contains(BlasterCharacter)) { HeadShotHitMap[BlasterCharacter]++; }
					else { HeadShotHitMap.Emplace(BlasterCharacter, 1); }
				}
				else
				{
					if (HitMap.Contains(BlasterCharacter)) { HitMap[BlasterCharacter]++; }
					else { HitMap.Emplace(BlasterCharacter, 1); }
				}
			}

			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					FireHit.ImpactPoint,
					FireHit.ImpactNormal.Rotation()
				);
			}
			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					HitSound,
					FireHit.ImpactPoint,
					0.5f,
					FMath::FRandRange(-0.5f, 0.5f)
				);
			}
		}

		TArray<ABlasterCharacter*> HitCharacters;
		
		//Maps Character hit to total damage
		TMap<ABlasterCharacter*, float> DamageMap;

		//Calculate body shot damage by multiplying times hit x Damage
		for (auto HitPair : HitMap)
		{
			if (HitPair.Key)
			{
				DamageMap.Emplace(HitPair.Key, HitPair.Value * Damage);

				HitCharacters.AddUnique(HitPair.Key);
			}
		}
		//Calculate head shot damage by multiplying times hit x HeadShotDamage - store in DamageMap
		for (auto HeadShotHitPair : HeadShotHitMap)
		{
			if (HeadShotHitPair.Key)
			{
				if (DamageMap.Contains(HeadShotHitPair.Key)) { DamageMap[HeadShotHitPair.Key] += HeadShotHitPair.Value * HeadShotDamage; }
				else { DamageMap.Emplace(HeadShotHitPair.Key, HeadShotHitPair.Value * HeadShotDamage); }

				HitCharacters.AddUnique(HeadShotHitPair.Key);
			}
		}

		//Loop through DamageMap to get total damage for each character
		for (auto DamagePair : DamageMap)
		{
			if (DamagePair.Key && InstigatorController)
			{
				bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
				if (HasAuthority() && bCauseAuthDamage)
				{
					UGameplayStatics::ApplyDamage(
						DamagePair.Key, // Character that was hit
						DamagePair.Value , // Damage calculated in the two for loops above
						InstigatorController,
						this,
						UDamageType::StaticClass()
					);
				}
			}
		}

		if (!HasAuthority() && bUseServerSideRewind)
		{
			UE_LOG(LogTemp, Warning, TEXT("bUseServerSideRewind FIRE"));
			BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(OwnerPawn) : BlasterOwnerCharacter;
			BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(InstigatorController) : BlasterOwnerController;
			if (BlasterOwnerController && BlasterOwnerCharacter && BlasterOwnerCharacter->GetLagCompensation() && BlasterOwnerCharacter->IsLocallyControlled())
			{
				BlasterOwnerCharacter->GetLagCompensation()->ShotgunServerScoreRequest(
					HitCharacters,
					Start,
					HitTargets,
					BlasterOwnerController->GetServerTime() - BlasterOwnerController->SingleTripTime,
					this
				);
			}
		}
	}
}

void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket == nullptr) { return; }

	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	const FVector TraceStart = SocketTransform.GetLocation();

	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

	

	for (uint32 i = 0; i< NumberOfPellets; i++)
	{

		const FVector RandVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		const FVector EndLocation = SphereCenter + RandVector;
		FVector ToEndLocation = EndLocation - TraceStart;
		ToEndLocation = TraceStart + ToEndLocation * TRACE_LENGTH / ToEndLocation.Size();

		HitTargets.Add(ToEndLocation);
	}
}
