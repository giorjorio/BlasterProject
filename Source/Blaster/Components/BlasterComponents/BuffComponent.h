// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"

class ABlasterCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	friend class ABlasterCharacter;
	UBuffComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	* Health buff
	*/
	void Heal(float HealAmount, float HealingTime);

	/*
	* Jump buff
	*/
	void BuffJump(float BuffJumpVelocity, float BuffTime);
	void SetInitialJumpVelocity(float Velocity);

	/*
	* Speed buff
	*/
	void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime);
	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);


protected:
	virtual void BeginPlay() override;
	void HealPampUp(float DeltaTime);

private:
	UPROPERTY()
	ABlasterCharacter* Character;

	/*
	* Health buff
	*/
	bool bHealing = false;
	float HealingRate = 0;
	float AmountToHeal = 0;

	/*
	* Jump buff
	*/
	float InitialJumpVelocity;
	FTimerHandle JumpBuffTimer;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastJumpBuff(float JumpVelocity);

	void ResetJump();

	/*
	* Speed buff
	*/
	float InitialBaseSpeed;
	float InitialCrouchSpeed;
	FTimerHandle SpeedBuffTimer;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float BaseSpeed, float CrouchSpeed);
	
	void ResetSpeeds();


public:	

		
};
