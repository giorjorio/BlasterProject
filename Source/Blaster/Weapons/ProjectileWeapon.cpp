
#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "Projectile.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	UWorld* World = GetWorld();
	if(MuzzleFlashSocket && World)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		//From muzzle flash socket to hit location from TraceUnderCrosshairs
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;

		AProjectile* SpawnedProjectile = nullptr;
		
		if (bUseServerSideRewind && bUseServerSideRewindDefault)
		{
			if (InstigatorPawn->HasAuthority()) // server
			{
				if (InstigatorPawn->IsLocallyControlled()) // server, host - uses replicated projectile
				{
					SpawnedProjectile= World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = false;
					SpawnedProjectile->SetDamage(Damage);
					SpawnedProjectile->SetHeadShotDamage(HeadShotDamage);
				}
				else // server, not locally controlled - spawn non-replicated projectile, SSR
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = true;
				}
			}
			else // client using SSR
			{
				if (InstigatorPawn->IsLocallyControlled()) // client, locally controlled - spawn non-replicated projectile, use SSR
				{

					SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = true;
					SpawnedProjectile->TraceStart = SocketTransform.GetLocation();
					SpawnedProjectile->InitialVelocity = SpawnedProjectile->GetActorForwardVector() * SpawnedProjectile->InitialSpeed;
				}
				else // client, not locally controlled - spawn non-replicated projecctile, no SSR
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = false;
				}
			}
		}
		else // Weapon not using SSR
		{
			if (InstigatorPawn->HasAuthority()) // server
			{
				SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
				SpawnedProjectile->bUseServerSideRewind = false;
				SpawnedProjectile->SetDamage(Damage);
				SpawnedProjectile->SetHeadShotDamage(HeadShotDamage);
			}
		}
	}
}
