
#include "ShieldPickup.h"

#include "Blaster/Components/BlasterComponents/BuffComponent.h"
#include "Blaster/Character/BlasterCharacter.h"

void AShieldPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor))
	{
		UBuffComponent* Buff = BlasterCharacter->GetBuff();
		if (Buff)
		{
			if (!Buff->ReplenishShield(ShieldReplenishAmount, ShieldReplenishTime)) { return; }
		}
	}
	Destroy();
}