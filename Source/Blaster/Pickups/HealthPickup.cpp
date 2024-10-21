
#include "HealthPickup.h"

#include "Blaster/Components/BlasterComponents/BuffComponent.h"
#include "Blaster/Character/BlasterCharacter.h"

void AHealthPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor))
	{
		UBuffComponent* Buff = BlasterCharacter->GetBuff();
		if (Buff)
		{
			if (GetWorldTimerManager().IsTimerActive(BlasterCharacter->PassiveHealingTimer))
			{
				GetWorldTimerManager().ClearTimer(BlasterCharacter->PassiveHealingTimer);
			}
			if (!Buff->Heal(HealAmount, HealingTime)) { return; }
		}
	}
	Destroy();
}
