// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "HartSaveGame.generated.h"


USTRUCT(BlueprintType)
struct FActorSaveData
{
	GENERATED_BODY()

	// 1. Le nom unique de l'acteur dans le niveau (pour le retrouver au chargement)
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FName ActorName;

	// 2. Sa position, rotation et taille
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FTransform Transform;

	// 3. La "boîte noire" : Toutes ses variables spécifiques converties en binaire
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<uint8> ByteData;
};


/**
 * 
 */
UCLASS()
class HARTEOSLIBRARY_API UHartSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public :
    // Identifiant unique pour le slot de SauvegardeGlobal
    UPROPERTY(VisibleAnywhere, Category = Basic)
    FString SaveSlotName;

    // Dictionnaire : Clé (FString : Nom de la save) -> Valeur (float : Temps de jeu)
    UPROPERTY(VisibleAnywhere, Category = "Basic")
    TMap<FString, float> MySaves;

};


/**
 * Ceci est la "Boîte" propre au jeu.
 * Elle contient toutes les données que tu veux conserver.
 */
UCLASS()
class HARTEOSLIBRARY_API UMyGameSaveData : public USaveGame
{
	GENERATED_BODY()

public:
	// le niveau actuel
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FString SavedLevelName;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<uint8> GameInstanceData;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<uint8> GameModeData;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<uint8> PlayerControllerData;

	//Le grand tableau qui contiendra tous tes acteurs !
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<FActorSaveData> SavedActors;
};