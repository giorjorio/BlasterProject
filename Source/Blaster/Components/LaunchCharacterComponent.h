// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "LaunchCharacterComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLASTER_API ULaunchCharacterComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	ULaunchCharacterComponent();

protected:

	virtual void BeginPlay() override;

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Jump, meta = (AllowPrivateAccess = "true"))
	FVector LaunchVelocity = FVector(0.f, 0.f, 1500.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Jump, meta = (AllowPrivateAccess = "true"))
	bool bXYoverride = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Jump, meta = (AllowPrivateAccess = "true"))
	bool bZOverride = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Jump, meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Jump, meta = (AllowPrivateAccess = "true"))
	FName AcceptableActorTag = TEXT("Launch");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Jump, meta = (AllowPrivateAccess = "true"))
	bool bUseCharacterDirection;

	/** called when something enters the sphere component */
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** called when something leaves the sphere component */
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, 
		class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
