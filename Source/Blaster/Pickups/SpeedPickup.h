#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "SpeedPickup.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class BLASTER_API ASpeedPickup : public APickup
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
	float BaseSpeedBuff = 1600.f;

	UPROPERTY(EditAnywhere)
	float CrouchSpeedBuff = 850.f;

	UPROPERTY(EditAnywhere)
	float SpeedBuffTime = 30.f;
};
