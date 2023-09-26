// Fill out your copyright notice in the Description page of Project Settings.


#include "LaunchCharacterComponent.h"
#include "Blaster/Character/BlasterCharacter.h"

ULaunchCharacterComponent::ULaunchCharacterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULaunchCharacterComponent::BeginPlay()
{
	Super::BeginPlay();

    OnComponentBeginOverlap.AddDynamic(this, &ULaunchCharacterComponent::OnOverlapBegin);
    OnComponentEndOverlap.AddDynamic(this, &ULaunchCharacterComponent::OnOverlapEnd);
}

void ULaunchCharacterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    
   
}

void ULaunchCharacterComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor != nullptr)
    {
        if (OtherActor->IsA(ABlasterCharacter::StaticClass()))
        {
            if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor))
            {
                if (bUseCharacterDirection)
                {
                    FVector ForwardVector = BlasterCharacter->GetActorForwardVector();
                    ForwardVector.Z = 1;
                    ForwardVector *= Speed;
                    BlasterCharacter->LaunchCharacter(ForwardVector, bXYoverride, bZOverride);
                }
                else
                {
                    BlasterCharacter->LaunchCharacter(LaunchVelocity, bXYoverride, bZOverride);
                }
                
            }
        }
        else if (OtherActor->ActorHasTag(AcceptableActorTag))
        {

            AActor* TargetActor = OtherActor;
            UPrimitiveComponent* Target = CastChecked<UPrimitiveComponent>(TargetActor->GetComponentByClass(UPrimitiveComponent::StaticClass()));
            Target->AddImpulse(LaunchVelocity);

        }
    }
}

void ULaunchCharacterComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}


