// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "TeleportComponent.generated.h"


class ABlasterCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLASTER_API UTeleportComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	UTeleportComponent();

protected:

	virtual void BeginPlay() override;

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teleport", meta = (AllowPrivateAccess = "true"))
	AActor* Destination;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teleport", meta = (AllowPrivateAccess = "true"))
	FVector DestinationLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teleport", meta = (AllowPrivateAccess = "true"))
	FRotator DestinationRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teleport", meta = (AllowPrivateAccess = "true"))
	FVector AdjustDestination;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teleport", meta = (AllowPrivateAccess = "true"))
	FRotator AdjustRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teleport", meta = (AllowPrivateAccess = "true"))
	bool bIsAnotherTeleport;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Teleport, meta = (AllowPrivateAccess = "true"))
	FName AcceptableActorTag = TEXT("Teleport");

	//bool bIsTeleporting;

	/** called when something enters the sphere component */
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** called when something leaves the sphere component */
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
			class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void Teleportation(ABlasterCharacter* BlasterCharacter);
};
