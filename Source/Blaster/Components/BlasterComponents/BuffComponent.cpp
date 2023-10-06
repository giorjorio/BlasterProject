
#include "BuffComponent.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
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
	SpeedFallingOff(DeltaTime);
	JumpFallingOff(DeltaTime);
}


/*
* Health buff
*/
bool UBuffComponent::CanBeHealed(float HealAmount)
{
	//Checking whether or player can pick up the heal pickup
	if (!bHealing)
	{
		if (Character)
		{
			CanHealAmount = Character->GetMaxHealth() - Character->GetHealth();
			CanHealAmount -= HealAmount;
			InitialHealthAmount = Character->GetHealth();
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
	InitialHealAmount += HealAmount;	

	return true;

}

void UBuffComponent::HealRampUp(float DeltaTime)
{
	if (!bHealing || Character == nullptr || Character->IsElimmed()) { return; }

	const float HealThisFrame = HealingRate * DeltaTime;
	AmountToHeal -= HealThisFrame;

	if (AmountToHeal <= 0.f || Character->GetHealth() >= Character->GetMaxHealth())
	{
		if (InitialHealAmount > Character->GetHealth() - InitialHealthAmount)
		{			
			// If the health is not healed enough, it will add the lost difference
			float diff = InitialHealAmount + InitialHealthAmount - Character->GetHealth();
			Character->SetHealth(FMath::FloorToInt32(FMath::Clamp(Character->GetHealth() + diff, 0.f, Character->GetMaxHealth())));
			Character->UpdateHUDHealth();
		}

		bHealing = false;
		AmountToHeal = 0.f;
		InitialHealAmount = 0.f;
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
	if (Character == nullptr ) { return; }

	Character->GetWorldTimerManager().SetTimer(
			JumpBuffTimer,
			this,
			&UBuffComponent::ResetJump,
			BuffTime
		);

	MulticastJumpBuff(BuffJumpVelocity, true, BuffTime);
}

void UBuffComponent::MulticastJumpBuff_Implementation(float JumpVelocity, bool bWork, float BuffTime)
{
	UE_LOG(LogTemp, Warning, TEXT("MulticastJumpBuff_Implementation"));

	if (Character == nullptr  || Character->GetCharacterMovement() == nullptr) { return; }

	if(bWork)
	{
		JumpPickupBuffTime = BuffTime;
		bJumpBuffed = true;
		JumpTimeLeft = JumpPickupBuffTime;
	}
	else
	{
		bJumpBuffed = false;
	}

	Character->GetCharacterMovement()->JumpZVelocity = JumpVelocity;
}

void UBuffComponent::JumpFallingOff(float DeltaTime)
{
	if (!bJumpBuffed || Character == nullptr || Character->Controller == nullptr || Character->IsElimmed()) { return; }

	//const float TimeLeft = Character->GetWorldTimerManager().GetTimerRemaining(JumpBuffTimer);

	JumpTimeLeft -= DeltaTime;

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->UpdateJumpBuffBar(JumpTimeLeft, JumpPickupBuffTime);
	}
}


void UBuffComponent::ResetJump()
{
	MulticastJumpBuff(InitialJumpVelocity, false);
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
	//Checking whether or player can pick up the shield pickup
	if (!bReplenishingShield)
	{
		if (Character)
		{
			CanReplenishAmount = Character->GetMaxShield() - Character->GetShield();
			CanReplenishAmount -= ShieldAmount;
			InitialShieldAmount = Character->GetShield();
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
	InitialReplenishAmount += ShieldAmount;

	return true;
}

void UBuffComponent::ShieldRampUp(float DeltaTime)
{
	if (!bReplenishingShield || Character == nullptr || Character->IsElimmed()) { return; }

	const float ReplenishThisFrame = ShieldReplenishRate * DeltaTime;
	ShieldReplenishAmount -= ReplenishThisFrame;

	if (ShieldReplenishAmount <= 0.f || Character->GetShield() >= Character->GetMaxShield())
	{
		if (InitialReplenishAmount > Character->GetShield()-InitialShieldAmount)
		{
			// If the shield is not replenished enough, it will add the lost difference
			float diff = InitialReplenishAmount + InitialShieldAmount - Character->GetShield();
			Character->SetShield(FMath::FloorToInt32(FMath::Clamp(Character->GetShield() + diff, 0.f, Character->GetMaxShield())));
			Character->UpdateHUDShield();
		}
		
		bReplenishingShield = false;
		ShieldReplenishAmount = 0.f;
		InitialReplenishAmount = 0.f;
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

	MulticastSpeedBuff(BuffBaseSpeed, BuffCrouchSpeed, true, BuffTime);
}

void UBuffComponent::MulticastSpeedBuff_Implementation(float BaseSpeed, float CrouchSpeed, bool bWork, float BuffTime)
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) { return; }

	if (bWork)
	{
		SpeedPickupBuffTime = BuffTime;
		bSpeedBuffed = true;
		SpeedTimeLeft = SpeedPickupBuffTime;
	}
	else
	{
		bSpeedBuffed = true;
	}
	Character->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;

}

void UBuffComponent::SpeedFallingOff(float DeltaTime)
{
	if (!bSpeedBuffed || Character == nullptr || Character->Controller == nullptr || Character->IsElimmed()) { return; }

	SpeedTimeLeft -= DeltaTime;

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->UpdateSpeedBuffBar(SpeedTimeLeft, SpeedPickupBuffTime);
	}
}

void UBuffComponent::ResetSpeeds()
{
	MulticastSpeedBuff(InitialBaseSpeed, InitialCrouchSpeed, false);
}

void UBuffComponent::SetInitialSpeeds(float BaseSpeed, float CrouchSpeed)
{
	InitialBaseSpeed = BaseSpeed;
	InitialCrouchSpeed = CrouchSpeed;
}




