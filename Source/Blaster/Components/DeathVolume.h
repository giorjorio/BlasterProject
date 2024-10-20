// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "DeathVolume.generated.h"



/**
 * 
 */

class ABlasterPlayerController;

UCLASS()
class BLASTER_API ADeathVolume : public ATriggerVolume
{
	GENERATED_BODY()

public:

	ADeathVolume();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;


private:

	ABlasterPlayerController* Controller;

	UPROPERTY(EditAnywhere)
	float Damage = 500.f;

	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);


};
