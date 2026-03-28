// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BuildingData.generated.h"

/**
 * 
 */
UCLASS()
class RAGNARIM_API UBuildingData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataBuilding")
	class UStaticMesh* BuildingStaticMesh;

	UPROPERTY(EditAnywhere)
	float BaseHealth = 100.0f;

	UPROPERTY(EditAnywhere)
	float BaseBuildValue = 5.0f;
};
