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
	bool Heal(float HealAmount, float HealingTime);

	/*
	* Jump buff
	*/
	void BuffJump(float BuffJumpVelocity, float BuffTime);
	void SetInitialJumpVelocity(float Velocity);

	/*
	* Shield Buff
	*/
	bool ReplenishShield(float ShieldAmount, float ReplenishTime);

	/*
	* Speed buff
	*/
	void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime);
	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);


protected:
	virtual void BeginPlay() override;

	/*
	* Health buff
	*/
	void HealRampUp(float DeltaTime);

	/*
	* Shield Buff
	*/
	void ShieldRampUp(float DeltaTime);

private:
	UPROPERTY()
	ABlasterCharacter* Character;

	/*
	* Health buff
	*/
	bool bHealing = false;
	float HealingRate = 0.f;
	float AmountToHeal = 0.f;
	float CanHealAmount = 0.f;
	bool CanBeHealed(float HealAmount);

	/*
	* Jump buff
	*/
	float InitialJumpVelocity;
	FTimerHandle JumpBuffTimer;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastJumpBuff(float JumpVelocity);

	void ResetJump();

	/*
	* Shield Buff
	*/
	bool bReplenishingShield = false;
	float ShieldReplenishRate = 0.f;
	float ShieldReplenishAmount = 0.f;
	float CanReplenishAmount = 0.f;
	bool CanBeReplenished(float ShieldAmount);
	
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
