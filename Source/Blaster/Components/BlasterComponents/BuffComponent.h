// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"

class ABlasterCharacter;
class ABlasterPlayerController;

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
	* Jump buff
	*/
	void JumpFallingOff(float DeltaTime);
	float JumpTimeLeft;

	/*
	* Shield Buff
	*/
	void ShieldRampUp(float DeltaTime);

	/*
	* Speed buff
	*/
	void SpeedFallingOff(float DeltaTime);
	float SpeedTimeLeft;

private:
	UPROPERTY()
	ABlasterCharacter* Character;

	UPROPERTY()
	ABlasterPlayerController* Controller;

	/*
	* Health buff
	*/
	bool bHealing = false;
	float HealingRate = 0.f;
	float AmountToHeal = 0.f;
	float CanHealAmount = 0.f;
	float InitialHealthAmount = 0.f;
	float InitialHealAmount = 0.f;
	bool CanBeHealed(float HealAmount);

	/*
	* Jump buff
	*/
	bool bJumpBuffed = false;
	float InitialJumpVelocity;
	float JumpPickupBuffTime;
	FTimerHandle JumpBuffTimer;
	

	UFUNCTION(NetMulticast, Reliable)
	void MulticastJumpBuff(float JumpVelocity, bool bWork = true, float BuffTime = 0.f);

	void ResetJump();

	/*
	* Shield Buff
	*/
	bool bReplenishingShield = false;
	float ShieldReplenishRate = 0.f;
	float ShieldReplenishAmount = 0.f;
	float CanReplenishAmount = 0.f;
	float InitialShieldAmount = 0.f;
	float InitialReplenishAmount = 0.f;
	bool CanBeReplenished(float ShieldAmount);
	
	/*
	* Speed buff
	*/
	bool bSpeedBuffed = false;
	float InitialBaseSpeed;
	float InitialCrouchSpeed;
	float SpeedPickupBuffTime;
	FTimerHandle SpeedBuffTimer;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float BaseSpeed, float CrouchSpeed, bool bWork = true, float BuffTime = 0.f);
	
	void ResetSpeeds();


public:	

		
};
