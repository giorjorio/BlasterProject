// Fill out your copyright notice in the Description page of Project Settings.


#include "Experiment.h"

#include "Components/StaticMeshComponent.h"

// Sets default values
AExperiment::AExperiment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	Cube = CreateAbstractDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
	Cube->SetupAttachment(RootComponent);
	SetRootComponent(Cube);

	Cube->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Cube->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Cube->SetSimulatePhysics(true);
	Cube->SetEnableGravity(true);

	Arrow = CreateAbstractDefaultSubobject<UStaticMeshComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(RootComponent);
	Arrow->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
	Arrow->SetUsingAbsoluteRotation(true);

	Arrow->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Arrow->SetCollisionEnabled(ECollisionEnabled::NoCollision);



}

// Called when the game starts or when spawned
void AExperiment::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExperiment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

