// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleBuilding.h"

// Sets default values
ASimpleBuilding::ASimpleBuilding()
{
    PrimaryActorTick.bCanEverTick = false;

	// Créer une racine
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponentForBuilding"));
	MeshComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ASimpleBuilding::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASimpleBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

