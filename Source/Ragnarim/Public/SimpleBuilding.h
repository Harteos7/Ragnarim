// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SimpleBuilding.generated.h"

UCLASS()
class RAGNARIM_API ASimpleBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASimpleBuilding();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Bool pour savoir si c'est une fondation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	bool bFondation = false;

	// Bool pour savoir si ca à besoin d'une fondation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	bool bNeedFondation = false;

	// Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// Le fichier 3D de référence pour ce bâtiment
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	class UStaticMesh* BuildingMeshAsset;

};
