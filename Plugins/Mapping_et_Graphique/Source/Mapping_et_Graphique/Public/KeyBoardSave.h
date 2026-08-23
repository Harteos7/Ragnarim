#pragma once

#include "CoreMinimal.h"
#include "Engine/Font.h"
#include "GameFramework/SaveGame.h"
#include "KeyBoardSave.generated.h"

USTRUCT(BlueprintType)
struct FInputActionKey
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyboardSettings")
    FString SaveIMC; // Nom du mapping context

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyboardSettings")
    FName ActionName; // Le nom de l'Input Action (ex: IA_Forward)

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyboardSettings")
    FKey SaveKey; // La nouvelle touche choisie

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyboardSettings")
    bool bHasShiftModifier;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyboardSettings")
    int32 SlotIndex = 0; // 0 pour la touche principale, 1 pour la secondaire, etc.

};

UCLASS()
class MAPPING_ET_GRAPHIQUE_API UKeyBoardSave : public USaveGame
{
    GENERATED_BODY()

public:
    // La langue du clavier ou config choisie par le joueur
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
    FString KeyboardLayout = "fr-FR"; // valeur par défaut

    // Tableau de touches avec FKey et FString
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
    TArray<FInputActionKey> SaveInputAction_Key;

    // Sauvegarder les sensibilités et autre réglages simple
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
    TMap<FString, float> SaveSimpleFloat;

    // Sauvegarder du font
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
    TSoftObjectPtr<UFont> FontSave;
};