// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingData.h"
#include "Resource/MaterialData.h"
#include "BaseBuilding.generated.h"

UCLASS(Abstract, Blueprintable)
class RAGNARIM_API ABaseBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseBuilding();

	// S'exécute quand on modifie l'objet dans l'éditeur ou quand il spawn
    virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UBuildingData* BuildingData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UMaterialData* SelectedMaterial;

	UPROPERTY(BlueprintReadWrite, Category = "Data")
	float MaxHealth;

	UPROPERTY(BlueprintReadWrite, Category = "Data")
	float MaxBuildValue;

};
