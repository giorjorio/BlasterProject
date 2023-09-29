#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawnPoint.generated.h"

class APickup;

UCLASS()
class BLASTER_API APickupSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	APickupSpawnPoint();
	virtual void Tick(float DeltaTime) override;


protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<APickup>> PickupClasses;

	void SpawnPickup();

private:


public:	

};
