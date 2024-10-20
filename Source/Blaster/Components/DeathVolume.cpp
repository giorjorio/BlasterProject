// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathVolume.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GamePlayStatics.h"




ADeathVolume::ADeathVolume()
{
}

void ADeathVolume::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &ADeathVolume::OnOverlapBegin);

}

void ADeathVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADeathVolume::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor != nullptr)
	{
		if (OtherActor->IsA(ABlasterCharacter::StaticClass()))
		{
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
			if (BlasterCharacter)
			{
				Controller = Cast<ABlasterPlayerController>(BlasterCharacter->GetController());
				if (Controller)
				{
					float DamageToDeath = BlasterCharacter->GetHealth() + BlasterCharacter->GetShield();

					if (DamageToDeath <= Damage && DamageToDeath != 1.f)
					{
						UGameplayStatics::ApplyDamage(
							BlasterCharacter,
							DamageToDeath - 1,
							Controller,
							this,
							UDamageType::StaticClass()
						);
					}
					else if (DamageToDeath == 1.f)
					{
						UGameplayStatics::ApplyDamage(
							BlasterCharacter,
							0.f,
							Controller,
							this,
							UDamageType::StaticClass()
						);
					}
					else
					{
						UGameplayStatics::ApplyDamage(
							BlasterCharacter,
							Damage,
							Controller,
							this,
							UDamageType::StaticClass()
						);
					}

					TArray<AActor*> PlayerStarts;
					UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
					int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);

					BlasterCharacter->SetActorLocation(PlayerStarts[Selection]->GetActorLocation());
					BlasterCharacter->SetActorRotation(PlayerStarts[Selection]->GetActorRotation());

					if(BlasterCharacter->IsLocallyControlled())
					{
						Controller->SetControlRotation(BlasterCharacter->GetActorRotation());
					}
				}
			}
		}
	}

	
}
