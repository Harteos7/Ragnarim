// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Resource/MaterialData.h"
#include "Building/BuildingData.h"
#include "Game/RagnarimTypes.h" 
#include "GameplayTagContainer.h"
#include "GameInstance_Ragnarim.generated.h"

// pour compiler plus vite
class UBuildingData;
class UMaterialData;

// Pour que les blueprints ont les maps 
USTRUCT(BlueprintType)
struct FBuildingList {
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly)
    TArray<UBuildingData*> Buildings;
};

USTRUCT(BlueprintType)
struct FMaterialList {
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly)
    TArray<UMaterialData*> Materials;
};


/**
 * 
 */
UCLASS()
class RAGNARIM_API UGameInstance_Ragnarim : public UGameInstance
{
	GENERATED_BODY()
	
public:
    // Initialisation équivalent du BeginPlay pour la GameInstance
    virtual void Init() override;

    // Lazy Accessor
    UFUNCTION(BlueprintPure, Category = "Ragnarim|Database", meta = (WorldContext = "WorldContextObject"))
    static UGameInstance_Ragnarim* Get(const UObject* WorldContextObject);





    // --- DATABASES ---

    // Bâtiments triés par Catégorie (Structure, Défense...)
    UPROPERTY(BlueprintReadOnly, Category = "Ragnarim|Database")
    TMap<EBuildingCategory, FBuildingList> BuildingDatabase;

    // Matériaux triés par Tag (Usage.Building, Usage.Crafting...)
    UPROPERTY(BlueprintReadOnly, Category = "Ragnarim|Database")
    TMap<FGameplayTag, FMaterialList> MaterialDatabase;





    // --- FONCTIONS ---

    UFUNCTION(BlueprintCallable, Category = "Ragnarim|Database")
    void RefreshAssetDatabase();

};
