// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "MaterialData.generated.h"

/**
 * 
 */
UCLASS()
class RAGNARIM_API UMaterialData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

    // --- VISUAL & UI ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Display")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Display")
    UTexture2D* Icon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
    UMaterialInterface* SurfaceMaterial;




    // --- LOGIC ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Logic")
    FGameplayTagContainer UsageTags;




    // --- STATS ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    float HealthMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    float BuildValueMultiplier = 1.0f;
};
