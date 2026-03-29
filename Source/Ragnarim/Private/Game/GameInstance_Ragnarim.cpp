// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameInstance_Ragnarim.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet/GameplayStatics.h"


UGameInstance_Ragnarim* UGameInstance_Ragnarim::Get(const UObject* WorldContextObject)
{
    // On utilise UGameplayStatics pour récupérer la Game Instance du monde actuel
    return Cast<UGameInstance_Ragnarim>(UGameplayStatics::GetGameInstance(WorldContextObject));
}

void UGameInstance_Ragnarim::Init()
{
    Super::Init();
    RefreshAssetDatabase();
}

void UGameInstance_Ragnarim::RefreshAssetDatabase()
{
    BuildingDatabase.Empty();
    MaterialDatabase.Empty();

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    TArray<FAssetData> AssetDataList;

    // --- 1. SCAN DES BÂTIMENTS ---
    AssetRegistryModule.Get().GetAssetsByClass(UBuildingData::StaticClass()->GetClassPathName(), AssetDataList);
    for (const FAssetData& Asset : AssetDataList)
    {
        if (UBuildingData* Data = Cast<UBuildingData>(Asset.GetAsset()))
        {
            BuildingDatabase.FindOrAdd(Data->Category).Buildings.Add(Data);
        }
    }

    AssetDataList.Empty(); // On vide la liste temporaire pour le prochain scan

    // --- 2. SCAN DES MATÉRIAUX ---
    AssetRegistryModule.Get().GetAssetsByClass(UMaterialData::StaticClass()->GetClassPathName(), AssetDataList);
    for (const FAssetData& Asset : AssetDataList)
    {
        if (UMaterialData* Data = Cast<UMaterialData>(Asset.GetAsset()))
        {
            // Un matériau peut avoir plusieurs tags (Bois = Construction + Craft)
            // On l'ajoute donc dans chaque catégorie correspondante de la Map
            for (const FGameplayTag& Tag : Data->UsageTags)
            {
                MaterialDatabase.FindOrAdd(Tag).Materials.Add(Data);
            }
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Database Ragnarim chargée : %d catégories de bâtiments."), BuildingDatabase.Num());
}