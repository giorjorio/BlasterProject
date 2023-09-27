// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoPickup.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Components/BlasterComponents/CombatComponent.h"

void AAmmoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor))
	{
		if (UCombatComponent* Combat = BlasterCharacter->GetCombat())
		{
			Combat->PickupAmmo(WeaponType, AmmoAmout);
		}
	}
	Destroy();
}
