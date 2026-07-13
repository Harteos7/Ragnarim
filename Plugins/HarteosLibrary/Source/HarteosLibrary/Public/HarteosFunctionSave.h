// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HartSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "HarteosFunctionSave.generated.h"


USTRUCT(BlueprintType)
struct FSave
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Sauvegarde")
	FString SaveName;

	UPROPERTY(BlueprintReadWrite, Category = "Sauvegarde")
	float TotalTimePlayed = 0.0f;

};

/**
 * 
 */
UCLASS()
class HARTEOSLIBRARY_API UHarteosFunctionSave : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	// Sauvegardes
	UFUNCTION(BlueprintCallable, Category = "Sauvegarde")
	static void AddSaveGame(FString MySaveName, float TotalTimePlayed, FString SavedLevelName, TArray<FActorSaveData> SavedActors, TArray<uint8> GameInstanceData, TArray<uint8> GameModeData, TArray<uint8> PlayerControllerData);

	static void CreateSaveSlots(FString MySaveName, float TotalTimePlayed);
	static bool AddToSaveSlots(FString MySaveName, float TotalTimePlayed);

	UFUNCTION(BlueprintCallable, Category = "Sauvegarde")
	static TArray<FSave> GetAllSaveGame();

	UFUNCTION(BlueprintCallable, Category = "Sauvegarde")
	static bool DeleteSaveGame(FString MySaveName);

	UFUNCTION(BlueprintCallable, Category = "Sauvegarde")
	static bool DeleteAllSaveGames();

	// ==========================
	// =  ECRITURE DES DONNÉES  =
	// ==========================

	// Serialize
	UFUNCTION(BlueprintCallable, Category = "Harteos Save System")
	static TArray<uint8> SerializeObjectToBytes(UObject* ObjectToSave);

	// Recherche des actors
	UFUNCTION(BlueprintCallable, Category = "Harteos Save System", meta = (WorldContext = "WorldContextObject"))
	static TArray<FActorSaveData> TriggerGetDataForSave(const UObject* WorldContextObject);


	// ============================
	// =  CHARGEMENT DES DONNÉES  =
	// ============================

	// Réinjecte les variables binaires dans un objet (GameMode, GameInstance, etc.)
	UFUNCTION(BlueprintCallable, Category = "Harteos Save System")
	static void DeserializeObjectFromBytes(UObject* ObjectToLoad, const TArray<uint8>& InByteData);

	// Parcourt le tableau de sauvegarde, retrouve les acteurs dans le niveau et les restaure
	UFUNCTION(BlueprintCallable, Category = "Harteos Save System", meta = (WorldContext = "WorldContextObject"))
	static void TriggerLoadActors(const UObject* WorldContextObject, const TArray<FActorSaveData>& SavedActors);

};
