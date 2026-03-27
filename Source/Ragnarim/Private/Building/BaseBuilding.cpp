// Fill out your copyright notice in the Description page of Project Settings.


#include "Building/BaseBuilding.h"

// Sets default values
ABaseBuilding::ABaseBuilding()
{
    PrimaryActorTick.bCanEverTick = false;

	// Créer une racine
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponentForBuilding"));
	MeshComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ABaseBuilding::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

