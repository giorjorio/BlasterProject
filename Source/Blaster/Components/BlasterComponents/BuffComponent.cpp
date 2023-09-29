
#include "BuffComponent.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


UBuffComponent::UBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;


}

void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HealRampUp(DeltaTime);
	ShieldRampUp(DeltaTime);

	if (Character && Character->GetCharacterMovement())
	{
		UE_LOG(LogTemp, Warning, TEXT("JumpZVelocity: %f"), Character->GetCharacterMovement()->JumpZVelocity);
		UE_LOG(LogTemp, Warning, TEXT("MaxWalkSpeed: %f"), Character->GetCharacterMovement()->MaxWalkSpeed);
	}
	
}


/*
* Health buff
*/
bool UBuffComponent::CanBeHealed(float HealAmount)
{
	if (!bHealing)
	{
		if (Character)
		{
			CanHealAmount = Character->GetMaxHealth() - Character->GetHealth();
			CanHealAmount -= HealAmount;
		}
		return true;
	}
	else
	{
		if (CanHealAmount <= 0)
		{
			return false;
		}
		else
		{
			CanHealAmount -= HealAmount;
			return true;
		}
	}
}

bool UBuffComponent::Heal(float HealAmount, float HealingTime)
{
	if (Character)
	{	
		// Don't pickup buff if the health is full
		if(Character->IsHPFull() )
		{
			return false;
		}
		else if (!CanBeHealed(HealAmount))
		{
			return false;
		}
	}
	

	bHealing = true;
	HealingRate = HealAmount / HealingTime;
	AmountToHeal += HealAmount;

	return true;
}

void UBuffComponent::HealRampUp(float DeltaTime)
{
	if (!bHealing || Character == nullptr || Character->IsElimmed()) { return; }

	const float HealThisFrame = HealingRate * DeltaTime;
	AmountToHeal -= HealThisFrame;

	if (AmountToHeal <= 0.f || Character->GetHealth() >= Character->GetMaxHealth())
	{
		bHealing = false;
		AmountToHeal = 0.f;
		return;
	}

	Character->SetHealth(FMath::Clamp(Character->GetHealth() + HealThisFrame, 0.f, Character->GetMaxHealth()));
	Character->UpdateHUDHealth();
}

/*
* Jump buff
*/
void UBuffComponent::BuffJump(float BuffJumpVelocity, float BuffTime)
{
	if (Character == nullptr) { return; }

	Character->GetWorldTimerManager().SetTimer(
		JumpBuffTimer,
		this,
		&UBuffComponent::ResetJump,
		BuffTime
	);

	if (Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->JumpZVelocity = BuffJumpVelocity;
	}
	MulticastJumpBuff(BuffJumpVelocity);
}

void UBuffComponent::MulticastJumpBuff_Implementation(float JumpVelocity)
{
	if (Character && Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->JumpZVelocity = JumpVelocity;
	}
}

void UBuffComponent::ResetJump()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) { return; }
	
	Character->GetCharacterMovement()->JumpZVelocity = InitialJumpVelocity;

	MulticastJumpBuff(InitialJumpVelocity);
}

void UBuffComponent::SetInitialJumpVelocity(float Velocity)
{
	InitialJumpVelocity = Velocity;
}

/*
* Shield Buff
*/
bool UBuffComponent::CanBeReplenished(float ShieldAmount)
{
	if (!bReplenishingShield)
	{
		if (Character)
		{
			CanReplenishAmount = Character->GetMaxShield() - Character->GetShield();
			CanReplenishAmount -= ShieldAmount;
		}
		return true;
	}
	else
	{
		if (CanReplenishAmount <= 0)
		{
			return false;
		}
		else
		{
			CanReplenishAmount -= ShieldAmount;
			return true;
		}
	}
}

bool UBuffComponent::ReplenishShield(float ShieldAmount, float ReplenishTime)
{
	if (Character)
	{
		// Don't pickup buff if the shield is full
		if (Character->IsShieldFull())
		{
			return false;
		}
		else if(!CanBeReplenished(ShieldAmount))
		{
			return false;
		}
	}

	bReplenishingShield = true;
	ShieldReplenishRate = ShieldAmount / ReplenishTime;
	ShieldReplenishAmount += ShieldAmount;
	
	return true;
}

void UBuffComponent::ShieldRampUp(float DeltaTime)
{
	if (!bReplenishingShield || Character == nullptr || Character->IsElimmed()) { return; }

	const float ReplenishThisFrame = ShieldReplenishRate * DeltaTime;
	ShieldReplenishAmount -= ReplenishThisFrame;

	if (ShieldReplenishAmount <= 0.f || Character->GetShield() >= Character->GetMaxShield())
	{
		bReplenishingShield = false;
		ShieldReplenishAmount = 0.f;
		return;
	}

	Character->SetShield(FMath::Clamp(Character->GetShield() + ReplenishThisFrame, 0.f, Character->GetMaxShield()));
	Character->UpdateHUDShield();

}

/*
* Speed buff
*/
void UBuffComponent::BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime)
{
	if (Character == nullptr) { return; }

	Character->GetWorldTimerManager().SetTimer(
		SpeedBuffTimer,
		this,
		&UBuffComponent::ResetSpeeds,
		BuffTime
	);

	if (Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BuffBaseSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = BuffCrouchSpeed;
	}
	MulticastSpeedBuff(BuffBaseSpeed, BuffCrouchSpeed);
}

void UBuffComponent::MulticastSpeedBuff_Implementation(float BaseSpeed, float CrouchSpeed)
{
	if (Character && Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
	}
}

void UBuffComponent::ResetSpeeds()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) { return; }

	Character->GetCharacterMovement()->MaxWalkSpeed = InitialBaseSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = InitialCrouchSpeed;

	MulticastSpeedBuff(InitialBaseSpeed, InitialCrouchSpeed);
}

void UBuffComponent::SetInitialSpeeds(float BaseSpeed, float CrouchSpeed)
{
	InitialBaseSpeed = BaseSpeed;
	InitialCrouchSpeed = CrouchSpeed;
}




