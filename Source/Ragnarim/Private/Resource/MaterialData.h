// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MaterialData.generated.h"

/**
 * 
 */
UCLASS()
class RAGNARIM_API UMaterialData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, Category = "Stats")
    FString MaterialName;

    UPROPERTY(EditAnywhere, Category = "Stats")
    float HealthMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Stats")
    float BuildValueMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Visual")
    UMaterialInterface* SurfaceMaterial;
};
