#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "HealthPickup.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class BLASTER_API AHealthPickup : public APickup
{
	GENERATED_BODY()

public:
	
protected:

	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;
	
private:

	UPROPERTY(EditAnywhere)
	float HealAmount = 50.f;

	UPROPERTY(EditAnywhere)
	float HealingTime = 5.f;

	
};
