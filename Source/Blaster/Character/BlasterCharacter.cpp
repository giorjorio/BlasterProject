// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"

#include "BlasterAnimInstance.h"
#include "Blaster/Blaster.h"
#include "Blaster/Components/BlasterComponents/BuffComponent.h"
#include "Blaster/Components/BlasterComponents/CombatComponent.h"
#include "Blaster/Components/BlasterComponents/LagCompensationComponent.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/Weapons/Flag.h"
#include "Blaster/Weapons/Weapon.h"
#include "Blaster/Weapons/WeaponTypes.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/WidgetComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"


ABlasterCharacter::ABlasterCharacter()
{

	PrimaryActorTick.bCanEverTick = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;

	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensation"));

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));
	
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	TurnSpeed = 5.f;

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	/*
	* Hit boxes used for server-side rewind
	*/
	HeadBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HeadBox"));
	HeadBox->SetupAttachment(GetMesh(), FName("head"));
	HitCollisionBoxes.Add(FName("HeadBox"), HeadBox);

	PelvisBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PelvisBox"));
	PelvisBox->SetupAttachment(GetMesh(), FName("pelvis"));
	HitCollisionBoxes.Add(FName("PelvisBox"), PelvisBox);

	Spine02Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Spine02Box"));
	Spine02Box->SetupAttachment(GetMesh(), FName("spine_02"));
	HitCollisionBoxes.Add(FName("Spine02Box"), Spine02Box);

	Spine03Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Spine03Box"));
	Spine03Box->SetupAttachment(GetMesh(), FName("spine_03"));
	HitCollisionBoxes.Add(FName("Spine03Box"), Spine03Box);

	UpperArmLeftBox = CreateDefaultSubobject<UBoxComponent>(TEXT("UpperArmLeftBox"));
	UpperArmLeftBox->SetupAttachment(GetMesh(), FName("upperarm_l"));
	HitCollisionBoxes.Add(FName("UpperArmLeftBox"), UpperArmLeftBox);

	UpperArmRightBox = CreateDefaultSubobject<UBoxComponent>(TEXT("UpperArmRightBox"));
	UpperArmRightBox->SetupAttachment(GetMesh(), FName("upperarm_r"));
	HitCollisionBoxes.Add(FName("UpperArmRightBox"), UpperArmRightBox);

	LowerArmLeftBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LowerArmLeftBox"));
	LowerArmLeftBox->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	HitCollisionBoxes.Add(FName("LowerArmLeftBox"), LowerArmLeftBox);

	LowerArmRightBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LowerArmRightBox"));
	LowerArmRightBox->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	HitCollisionBoxes.Add(FName("LowerArmRightBox"), LowerArmRightBox);

	HandLeftBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HandLeftBox"));
	HandLeftBox->SetupAttachment(GetMesh(), FName("hand_l"));
	HitCollisionBoxes.Add(FName("HandLeftBox"), HandLeftBox);

	HandRightBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HandRightBox"));
	HandRightBox->SetupAttachment(GetMesh(), FName("hand_r"));
	HitCollisionBoxes.Add(FName("HandRightBox"), HandRightBox);

	BackpackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BackpackBox"));
	BackpackBox->SetupAttachment(GetMesh(), FName("backpack"));
	HitCollisionBoxes.Add(FName("BackpackBox"), BackpackBox);

	BlanketBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BlanketBox"));
	BlanketBox->SetupAttachment(GetMesh(), FName("backpack"));
	HitCollisionBoxes.Add(FName("BlanketBox"), BlanketBox);

	ThighLeftBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ThighLeftBox"));
	ThighLeftBox->SetupAttachment(GetMesh(), FName("thigh_l"));
	HitCollisionBoxes.Add(FName("ThighLeftBox"), ThighLeftBox);

	ThighRightBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ThighRightBox"));
	ThighRightBox->SetupAttachment(GetMesh(), FName("thigh_r"));
	HitCollisionBoxes.Add(FName("ThighRightBox"), ThighRightBox);

	CalfLeftBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CalfLeftBox"));
	CalfLeftBox->SetupAttachment(GetMesh(), FName("calf_l"));
	HitCollisionBoxes.Add(FName("CalfLeftBox"), CalfLeftBox);

	CalfRightBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CalfRightBox"));
	CalfRightBox->SetupAttachment(GetMesh(), FName("calf_r"));
	HitCollisionBoxes.Add(FName("CalfRightBox"), CalfRightBox);

	FootLeftBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FootLeftBox"));
	FootLeftBox->SetupAttachment(GetMesh(), FName("foot_l"));
	HitCollisionBoxes.Add(FName("FootLeftBox"), FootLeftBox);

	FootRightBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FootRightBox"));
	FootRightBox->SetupAttachment(GetMesh(), FName("foot_r"));
	HitCollisionBoxes.Add(FName("FootRightBox"), FootRightBox);

	for (auto Box : HitCollisionBoxes)
	{
		if(Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_HitBox);
			Box.Value->SetCollisionResponseToAllChannels(ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

}

void ABlasterCharacter::AddInputMappinContextToPlayer()
{
	if (const ULocalPlayer* BlasterPlayer = (GEngine && GetWorld()) ? GEngine->GetFirstGamePlayer(GetWorld()) : nullptr)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(BlasterPlayer))
		{
			Subsystem->AddMappingContext(BlasterMappingContext, 0);
		}
	}
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (!HasAuthority())
	{
		AddInputMappinContextToPlayer();
	}

	CreateRoundProgressBars();
	SpawnDefaultWeapon();
	UpdateHUDHealth();
	UpdateHUDShield();
	UpdateHUDAmmo();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ABlasterCharacter::ReceiveDamage);
	}

	if (AttachedGrenade)
	{
		AttachedGrenade->SetVisibility(false);
	}
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ABlasterCharacter, Health);
	DOREPLIFETIME(ABlasterCharacter, Shield);
	DOREPLIFETIME(ABlasterCharacter, bDisableCharacterGameplay);
}

void ABlasterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AddInputMappinContextToPlayer();
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
	if (Buff)
	{
		Buff->Character = this;
		Buff->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
		Buff->SetInitialSpeeds(GetCharacterMovement()->MaxWalkSpeed, GetCharacterMovement()->MaxWalkSpeedCrouched);
	}
	if (LagCompensation)
	{
		LagCompensation->Character = this;
		if (Controller)
		{
			LagCompensation->Controller = Cast<ABlasterPlayerController>(Controller);
		}
	}
}

void ABlasterCharacter::PollInit()
{
	if (BlasterPlayerState == nullptr)
	{
		BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
		if (BlasterPlayerState)
		{
			BlasterPlayerState->AddToScore(0.f);
			BlasterPlayerState->AddToDefeats(0);
			SetTeamColor(BlasterPlayerState->GetTeam());

			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
			if (BlasterGameState && BlasterGameState->TopScoringPlayers.Contains(BlasterPlayerState))
			{
				MulticastGainedTheLead();
			}
		}
	}
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateInPlace(DeltaTime);
	HideCharacterIfCameraClose();
	PollInit();
}

/*
* AimOffset and Animation
*/
void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr)
	{
		return;
	}

	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir)//standing still
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			AO_YawInterp = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir)//running or jumping
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	CalculateAO_Pitch();
}

void ABlasterCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		//map pitch from [270, 360) to [-90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

float ABlasterCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

void ABlasterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void ABlasterCharacter::RotateInPlace(float DeltaTime)
{
	if (IsHoldingTheFlag())
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	if (bDisableCharacterGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return; 
	}

	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
}

void ABlasterCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) { return; }

	bRotateRootBone = false;

	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}


	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;


}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	//UE_LOG(LogTemp, Warning, TEXT("AO_Yaw: %f"), AO_Yaw);
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		AO_YawInterp = FMath::FInterpTo(AO_YawInterp, 0.f, DeltaTime, TurnSpeed);
		AO_Yaw = AO_YawInterp;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

/*
* Camera
*/
void ABlasterCharacter::HideCharacterIfCameraClose()
{
	if (!IsLocallyControlled()) { return; }
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		HideCharacterMesh(true);
	}
	else
	{
		HideCharacterMesh(false);
	}
}

void ABlasterCharacter::HideCharacterMesh(bool bHide)
{
	GetMesh()->SetVisibility(!bHide);
	if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
	{
		Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = bHide;

		if(Combat->SecondaryWeapon && Combat->SecondaryWeapon->GetWeaponMesh())
		{
			Combat->SecondaryWeapon->GetWeaponMesh()->bOwnerNoSee = bHide;
		}
	}
}

/*
* Default Weapon
*/
void ABlasterCharacter::SpawnDefaultWeapon()
{
	BlasterGameMode = BlasterGameMode == nullptr ? GetWorld()->GetAuthGameMode<ABlasterGameMode>() : BlasterGameMode;
	UWorld* World = GetWorld();
	if (BlasterGameMode && World && !bElimmed && DefaultWeaponClass)
	{
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->bDestroyWeapon = true;
		if (Combat)
		{
			Combat->EquipWeapon(StartingWeapon);
		}
	}
}

/*
* Dissolve Effect
*/
void ABlasterCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ABlasterCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void ABlasterCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

/*
* Elimination
*/
void ABlasterCharacter::DropOrDestroyWeapon(AWeapon* Weapon)
{
	if(Weapon)
	{
		if (Weapon->bDestroyWeapon)
		{
			Weapon->Destroy();
		}
		else
		{
			Weapon->Dropped();
		}
		if (Combat->TheFlag)
		{
			Combat->TheFlag->Dropped();
		}
	}
}

void ABlasterCharacter::DropOrDestroyWeapons()
{
	if (Combat)
	{
		if (Combat->EquippedWeapon)
		{
			Combat->UpdatePrimaryWeaponIcon(0.f);
			DropOrDestroyWeapon(Combat->EquippedWeapon);
		}
		if (Combat->SecondaryWeapon)
		{
			Combat->UpdateSecondaryWeaponIcon(0.f);
			DropOrDestroyWeapon(Combat->SecondaryWeapon);
		}
	}
}

void ABlasterCharacter::Destroyed()
{
	Super::Destroyed();

	if (ElimBotComponent)
	{
		ElimBotComponent->DestroyComponent();
	}
	BlasterGameMode = BlasterGameMode == nullptr ? GetWorld()->GetAuthGameMode<ABlasterGameMode>() : BlasterGameMode;
	bool bMatchNotInProgress = BlasterGameMode && BlasterGameMode->GetMatchState() != MatchState::InProgress;
	if (Combat && Combat->EquippedWeapon && bMatchNotInProgress)
	{
		Combat->EquippedWeapon->Destroy();
	}

}

void ABlasterCharacter::Elim(bool bPlayerLeftGame)
{
	DropOrDestroyWeapons();
	MulticastElim(bPlayerLeftGame);
}

void ABlasterCharacter::MulticastElim_Implementation(bool bPlayerLeftGame)
{
	bLeftGame = bPlayerLeftGame;

	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDWeaponAmmo(0);
		BlasterPlayerController->SetHUDEquippedWeaponIcon(0.f, true);
		BlasterPlayerController->SetHUDEquippedWeaponIcon(0.f, false);
	}

	bElimmed = true;
	PlayElimMontage();
	//Start dissolve effect
	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 200.f);
	}
	StartDissolve();


	//Disable character movement
	bDisableCharacterGameplay = true;
	GetCharacterMovement()->DisableMovement();
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
	//GetCharacterMovement()->StopMovementImmediately();

	//Disable collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Spawn elim bot
	if (ElimBotEffect)
	{
		FVector ElimBotSpawnPoint(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.f);
		ElimBotComponent = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ElimBotEffect,
			ElimBotSpawnPoint,
			GetActorRotation()
		);

		if (ElimBotSound)
		{
			UGameplayStatics::SpawnSoundAtLocation(
				GetWorld(),
				ElimBotSound,
				ElimBotSpawnPoint,
				FRotator::ZeroRotator
			);
		}
	}
	bool bHideSniperScope = IsLocallyControlled() && 
		Combat && 
		Combat->bAiming && 
		Combat->EquippedWeapon && 
		Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;

	if (bHideSniperScope)
	{
		ShowSniperScopeWidget(false);
	}
	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&ABlasterCharacter::ElimTimerFinished,
		ElimDelay);
}

void ABlasterCharacter::ElimTimerFinished()
{
	BlasterGameMode = BlasterGameMode == nullptr ? GetWorld()->GetAuthGameMode<ABlasterGameMode>() : BlasterGameMode;
	if (BlasterGameMode && !bLeftGame)
	{
		BlasterGameMode->RequestRespawn(this, Controller);
	}
	if (bLeftGame && IsLocallyControlled())
	{
		OnLeftGame.Broadcast();
	}
}

/*
* Health
*/
void ABlasterCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();

	if (Health < LastHealth && !bElimmed)
	{
		PlayHitReactMontage();
	}
}

void ABlasterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	BlasterGameMode = BlasterGameMode == nullptr ? GetWorld()->GetAuthGameMode<ABlasterGameMode>() : BlasterGameMode;
	if (bElimmed || BlasterGameMode == nullptr) { return; }
	Damage = BlasterGameMode->CalculateDamage(InstigatorController, Controller, Damage);

	float DamageToHealth = Damage;
	if (Shield > 0.f)
	{
		if (Shield >= Damage)
		{
			Shield = FMath::Clamp(Shield - Damage, 0.f, MaxShield);
			DamageToHealth = 0;
		}
		else
		{
			DamageToHealth = FMath::Clamp(DamageToHealth - Shield, 0.f, Damage);
			Shield = 0.f;
		}
	}

	Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);
	
	UpdateHUDHealth();
	UpdateHUDShield();
	PlayHitReactMontage();

	if (Health == 0.f)
	{
		if (BlasterGameMode)
		{
			BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
			ABlasterPlayerController* AttackerController = Cast<ABlasterPlayerController>(InstigatorController);
			BlasterGameMode->PlayerEliminated(this, BlasterPlayerController, AttackerController);
		}
	}
}

void ABlasterCharacter::UpdateHUDHealth()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(GetController()) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

/*
* HUD
*/
void ABlasterCharacter::CreateRoundProgressBars()
{

	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(GetController()) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDRoundProgressBars();
	}
}

void ABlasterCharacter::UpdateHUDAmmo()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController && Combat && Combat->EquippedWeapon)
	{
		BlasterPlayerController->SetHUDCarriedAmmo(Combat->CarriedAmmo);
		BlasterPlayerController->SetHUDWeaponAmmo(Combat->EquippedWeapon->GetAmmo());
	}
}

/*
* Leadership effects
*/
void ABlasterCharacter::MulticastGainedTheLead_Implementation()
{
	if (CrownSystem == nullptr) { return; }
	if (CrownComponent == nullptr)
	{
		CrownComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			CrownSystem,
			GetCapsuleComponent(),
			FName(),
			GetActorLocation() + FVector(0.f, 0.f, 110.f),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
	if (CrownComponent)
	{
		CrownComponent->Activate();
	}
}

void ABlasterCharacter::MulticastLostTheLead_Implementation()
{
	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}
}

/*
* Leaving the game
*/
void ABlasterCharacter::ServerLeaveGame_Implementation()
{
	BlasterGameMode = BlasterGameMode == nullptr ? GetWorld()->GetAuthGameMode<ABlasterGameMode>() : BlasterGameMode;
	BlasterPlayerState = BlasterPlayerState == nullptr ? GetPlayerState<ABlasterPlayerState>() : BlasterPlayerState;
	if (BlasterGameMode && BlasterPlayerState)
	{
		BlasterGameMode->PlayerLeftGame(BlasterPlayerState);
	}
}

/*
* Montages
*/
void ABlasterCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr || Combat->CombatState != ECombatState::ECS_Unoccupied)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;
		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("RocketLauncher");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("Shotgun");
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("SniperRifle");
			break;
		case EWeaponType::EWT_GrenadeLauncher:
			SectionName = FName("GrenadeLauncher");
			break;

		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlaySwapMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SwapMontage)
	{
		AnimInstance->Montage_Play(SwapMontage);
	}
}

void ABlasterCharacter::PlayThrowGrenadeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ThrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
		
	}	
}

/*
* Shield
*/
void ABlasterCharacter::OnRep_Shield(float LastShield)
{
	UpdateHUDShield();

	if (Shield < LastShield && !bElimmed)
	{
		PlayHitReactMontage();
	}
}

void ABlasterCharacter::UpdateHUDShield()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(GetController()) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDShield(Shield, MaxShield);
	}
}

/*
* Team
*/
void ABlasterCharacter::SetTeamColor(ETeam Team)
{
	if (GetMesh() == nullptr || OriginalMaterial == nullptr) { return; }

	switch (Team)
	{
	case ETeam::ET_NoTeam:
		GetMesh()->SetMaterial(0, OriginalMaterial);
		DissolveMaterialInstance = BlueDissolveMatInst;
		break;
	case ETeam::ET_BlueTeam:
		GetMesh()->SetMaterial(0, BlueMaterial);
		DissolveMaterialInstance = BlueDissolveMatInst;
		break;
	case ETeam::ET_RedTeam:
		GetMesh()->SetMaterial(0, RedMaterial);
		DissolveMaterialInstance = RedDissolveMatInst;
		break;
	default:
		break;
	}
}

/*
* Weapon
*/
void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

void ABlasterCharacter::ServerSwapWeaponsPressed_Implementation()
{
	if (Combat && Combat->ShouldSwapWeapons())
	{
		Combat->SwapWeapons();
	}
}

/*
* Getters
*/
bool ABlasterCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

bool ABlasterCharacter::IsHoldingTheFlag() const
{
	return (Combat && Combat->bHoldingTheFlag);
}

bool ABlasterCharacter::IsLocallyReloading()
{
	return (Combat && Combat->bLocallyReloading);
}

bool ABlasterCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

ECombatState ABlasterCharacter::GetCombatState() const
{
	if (Combat == nullptr)
	{
		return ECombatState::ECS_MAX;
	}
	return Combat->CombatState;
}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr)
	{
		return nullptr;
	}
	return Combat->EquippedWeapon;
}

FVector ABlasterCharacter::GetHitTarget() const
{
	if (Combat == nullptr)
	{
		return FVector();
	}
	return Combat->HitTarget;
}

ETeam ABlasterCharacter::GetTeam()
{
	BlasterPlayerState = BlasterPlayerState == nullptr ? GetPlayerState<ABlasterPlayerState>() : BlasterPlayerState;
	if (BlasterPlayerState)
	{
		return BlasterPlayerState->GetTeam();
	}
	return ETeam::ET_NoTeam;
}


/*
* Setters
*/
void ABlasterCharacter::SetDisableCharacterGameplay(bool DisableGameplay)
{
	bDisableCharacterGameplay = DisableGameplay;
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

/*
* Input
*/
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABlasterCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Look);

		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ABlasterCharacter::AimButtonPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ABlasterCharacter::AimButtonReleased);

		EnhancedInputComponent->BindAction(CrouchHoldAction, ETriggerEvent::Started, this, &ABlasterCharacter::CrouchButtonPressed);
		EnhancedInputComponent->BindAction(CrouchHoldAction, ETriggerEvent::Completed, this, &ABlasterCharacter::CrouchButtonReleased);
		EnhancedInputComponent->BindAction(CrouchPressedAction, ETriggerEvent::Started, this, &ABlasterCharacter::CrouchButtonPressed);

		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Equip);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ABlasterCharacter::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ABlasterCharacter::FireButtonReleased);

		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::ReloadButtonPressed);

		EnhancedInputComponent->BindAction(SwapWeaponsAction, ETriggerEvent::Started, this, &ABlasterCharacter::SwapWeaponsPressed);

		EnhancedInputComponent->BindAction(ThrowGrenadeAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::ThrowGrenadeButtonPressed);
	}

}

void ABlasterCharacter::AimButtonPressed()
{
	if (bDisableCharacterGameplay || IsHoldingTheFlag()) { return; }

	if (Combat && Combat->EquippedWeapon)
	{	
		Combat->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased()
{
	if (bDisableCharacterGameplay || IsHoldingTheFlag()) { return; }

	if (Combat && Combat->EquippedWeapon)
	{
		Combat->SetAiming(false);
	}
}

void ABlasterCharacter::CrouchButtonPressed()
{
	if (bDisableCharacterGameplay || IsHoldingTheFlag()) { return; }

	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void ABlasterCharacter::CrouchButtonReleased()
{
	if (bDisableCharacterGameplay || IsHoldingTheFlag()) { return; }

	if (bIsCrouched)
	{
		UnCrouch();
	}
}

void ABlasterCharacter::Equip()
{
	if (bDisableCharacterGameplay || IsHoldingTheFlag()) { return; }

	if(Combat && Combat->CombatState == ECombatState::ECS_Unoccupied)
	{
		ServerEquipButtonPressed();
	}
}

void ABlasterCharacter::FireButtonPressed()
{
	if (bDisableCharacterGameplay || IsHoldingTheFlag()) { return; }

	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void ABlasterCharacter::FireButtonReleased()
{
	if (bDisableCharacterGameplay || IsHoldingTheFlag()) { return; }

	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void ABlasterCharacter::Jump()
{
	if (bDisableCharacterGameplay || IsHoldingTheFlag()) { return; }

	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void ABlasterCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (GetController())
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABlasterCharacter::Move(const FInputActionValue& Value)
{
	if (bDisableCharacterGameplay) { return; }

	FVector2D MovementVector = Value.Get<FVector2D>();

	if (GetController())
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

		/////Simplier way/////
		/*const FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, MovementVector.Y);
		const FVector Right = GetActorRightVector();
		AddMovementInput(Right, MovementVector.X);*/	
	}
}

void ABlasterCharacter::ReloadButtonPressed()
{
	if (bDisableCharacterGameplay || IsHoldingTheFlag()) { return; }

	if (Combat)
	{
		Combat->Reload();
	}
}

void ABlasterCharacter::SwapWeaponsPressed()
{
	if (bDisableCharacterGameplay || IsHoldingTheFlag()) { return; }

	ServerSwapWeaponsPressed();
	if (Combat && Combat->ShouldSwapWeapons() && !HasAuthority() && Combat->CombatState == ECombatState::ECS_Unoccupied)
	{
		PlaySwapMontage();
		Combat->CombatState = ECombatState::ECS_SwappingWeapons;
		bFinishedSwapping = false;
	}
}

void ABlasterCharacter::ThrowGrenadeButtonPressed()
{
	if (bDisableCharacterGameplay || IsHoldingTheFlag()) { return; }

	if (Combat)
	{
		Combat->ThrowGrenade();
	}
}

