#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

class AProjectile;

UCLASS()
class BLASTER_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& HitTarget) override;

private:

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> ProjectileClass;
	
};
