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

void ABaseBuilding::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    // On vérifie si les donnée sont bonne dans les data asset
    if (BuildingData && BuildingData->BuildingStaticMesh && SelectedMaterial && MeshComponent)
    {
        MeshComponent->SetStaticMesh(BuildingData->BuildingStaticMesh);

        MeshComponent->SetMaterial(0, SelectedMaterial->SurfaceMaterial);

        MaxHealth = BuildingData->BaseHealth * SelectedMaterial->HealthMultiplier;
        MaxBuildValue = BuildingData->BaseBuildValue * SelectedMaterial->BuildValueMultiplier;
    }
    else if (GEngine)
    {
        FString ErrorMsg = FString::Printf(TEXT("Erreur sur %s : "), *GetName());

        if (!BuildingData)
            ErrorMsg += TEXT("BuildingData est NULL. ");
        else if (!BuildingData->BuildingStaticMesh)
            ErrorMsg += TEXT("Mesh dans BuildingData est NULL. ");

        if (!SelectedMaterial)
            ErrorMsg += TEXT("SelectedMaterial est NULL. ");

        if (!MeshComponent)
            ErrorMsg += TEXT("MeshComponent est NULL.");

        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, ErrorMsg);

        // Optionnel : Log aussi dans la console (Output Log) pour garder une trace
        UE_LOG(LogTemp, Warning, TEXT("%s"), *ErrorMsg);
    }
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

