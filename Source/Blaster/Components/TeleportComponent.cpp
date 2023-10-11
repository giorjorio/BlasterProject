// Fill out your copyright notice in the Description page of Project Settings.


#include "TeleportComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"

UTeleportComponent::UTeleportComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTeleportComponent::BeginPlay()
{
	Super::BeginPlay();

	//if(GetOwner()->HasAuthority())
	//{
		OnComponentBeginOverlap.AddDynamic(this, &UTeleportComponent::OnOverlapBegin);
		OnComponentEndOverlap.AddDynamic(this, &UTeleportComponent::OnOverlapEnd);
	//}
}

void UTeleportComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTeleportComponent::Teleportation(ABlasterCharacter* BlasterCharacter)
{
	if(BlasterCharacter && BlasterCharacter->GetController())
	{
		if (bIsAnotherTeleport)
		{
			BlasterCharacter->SetActorLocation(Destination->GetActorLocation() + AdjustDestination);
			BlasterCharacter->SetActorRotation(Destination->GetActorRotation() + AdjustRotation);
			
			Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(BlasterCharacter->GetController()) : Controller;
			if (Controller)
			{
				Controller->SetControlRotation(BlasterCharacter->GetActorRotation());
			}
			Controller = nullptr;
		}
		else
		{
			BlasterCharacter->SetActorLocation(DestinationLocation);
			BlasterCharacter->SetActorRotation(DestinationRotation);
			
			Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(BlasterCharacter->GetController()) : Controller;
			if (Controller)
			{
				Controller->SetControlRotation(BlasterCharacter->GetActorRotation());
			}
			Controller = nullptr;
		}
	}
}



void UTeleportComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag(AcceptableActorTag))
	{
		if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor))
		{
			Teleportation(BlasterCharacter);
		}
	}
}

void UTeleportComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}




