// Fill out your copyright notice in the Description page of Project Settings.


#include "HarteosFunctionSave.h"
#include "Engine/Level.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Serialization/MemoryWriter.h"
#include "Kismet/GameplayStatics.h"
#include "SaveableActor.h"




// =========================
// =    CreateSaveSlot     =
// =========================
void UHarteosFunctionSave::CreateSaveSlots(FString MySaveName, float TotalTimePlayed)
{
	UHartSaveGame* SaveSlotMySaves = Cast<UHartSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UHartSaveGame::StaticClass())
	);

	if (SaveSlotMySaves)
	{
		SaveSlotMySaves->MySaves.Add(MySaveName, TotalTimePlayed);
		UGameplayStatics::SaveGameToSlot(SaveSlotMySaves, "SaveSlotMySaves", 0);
	}
}






// =========================
// =     AddToSaveSlots    =
// =========================
bool UHarteosFunctionSave::AddToSaveSlots(FString MySaveName, float TotalTimePlayed)
{
	UHartSaveGame* LoadedGame = Cast<UHartSaveGame>(
		UGameplayStatics::LoadGameFromSlot("SaveSlotMySaves", 0)
	);

	if (!LoadedGame) return false;

	// Magie du TMap : s'il existe déjà, ça met le temps à jour. Sinon, ça le crée.
	LoadedGame->MySaves.Add(MySaveName, TotalTimePlayed);

	return UGameplayStatics::SaveGameToSlot(LoadedGame, "SaveSlotMySaves", 0);

	return true;
}







// =========================
// =       AddSaveGame     =
// =========================
void UHarteosFunctionSave::AddSaveGame(FString MySaveName, float TotalTimePlayed, FString SavedLevelName, TArray<FActorSaveData> SavedActors, TArray<uint8> GameInstanceData, TArray<uint8> GameModeData, TArray<uint8> PlayerControllerData)
{
	UMyGameSaveData* MySaves = Cast<UMyGameSaveData>(
		UGameplayStatics::CreateSaveGameObject(UMyGameSaveData::StaticClass())
	);

	if (!MySaves)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("ERREUR: echec de la création de la sauvegarde MySaves dans AddSaveGame"));
		return;
	}

	MySaves->SavedLevelName = SavedLevelName;
	MySaves->SavedActors = SavedActors;
	MySaves->GameInstanceData = GameInstanceData;
	MySaves->GameModeData = GameModeData;
	MySaves->PlayerControllerData = PlayerControllerData;

	// L'appel magique d'Unreal : ça crée le fichier s'il est nouveau, ou ça l'écrase s'il existait déjà !
	bool bSuccess = UGameplayStatics::SaveGameToSlot(MySaves, MySaveName, 0);

	if (bSuccess)
	{
		// Une fois la partie sauvegardée, on met à jour notre index "SaveSlotMySaves"
		if (!UGameplayStatics::DoesSaveGameExist("SaveSlotMySaves", 0))
		{
			CreateSaveSlots(MySaveName, TotalTimePlayed);
		}
		else
		{
			AddToSaveSlots(MySaveName, TotalTimePlayed);
		}
	}
	else if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("ÉCHEC de la sauvegarde pour le slot: %s"), *MySaveName));
	}
}





// Function HUD

// =========================
// =    GetAllSaveGame     =
// =========================
TArray<FSave> UHarteosFunctionSave::GetAllSaveGame()
{
	TArray<FSave> AllSaveMetadata;

	// 1. On charge directement le fichier maître (Remplace l'ancien Reload_Save)
	UHartSaveGame* MasterSave = Cast<UHartSaveGame>(UGameplayStatics::LoadGameFromSlot("SaveSlotMySaves", 0));

	// Si le fichier maître n'existe pas, on retourne simplement un tableau vide
	if (!MasterSave)
	{
		return AllSaveMetadata;
	}

	// 2. On parcourt toutes les sauvegardes enregistrées dans le fichier maître
	for (const TPair<FString, float>& SaveEntry : MasterSave->MySaves)
	{
		FSave SaveData;
		SaveData.SaveName = SaveEntry.Key;
		SaveData.TotalTimePlayed = SaveEntry.Value;

		AllSaveMetadata.Add(SaveData);
	}

	return AllSaveMetadata;
}


// =========================
// =    DeleteSaveGame     =
// =========================
bool UHarteosFunctionSave::DeleteSaveGame(FString MySaveName)
{
	// 1. On supprime physiquement le fichier du disque dur (Unreal s'occupe de tout)
	bool bIsDeleted = UGameplayStatics::DeleteGameInSlot(MySaveName, 0);

	// 2. On met à jour le fichier maître pour qu'il oublie ce nom
	if (UHartSaveGame* MasterSave = Cast<UHartSaveGame>(UGameplayStatics::LoadGameFromSlot("SaveSlotMySaves", 0)))
	{
		MasterSave->MySaves.Remove(MySaveName);
		UGameplayStatics::SaveGameToSlot(MasterSave, "SaveSlotMySaves", 0);
	}
	else if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Avertissement: Impossible de cast le MasterSave sur UHartSaveGame."));
	}

	return bIsDeleted;
}


// =========================
// =   DeleteAllSaveGames  =
// =========================
bool UHarteosFunctionSave::DeleteAllSaveGames()
{
	if (UHartSaveGame* MasterSave = Cast<UHartSaveGame>(UGameplayStatics::LoadGameFromSlot("SaveSlotMySaves", 0)))
	{
		// On parcourt le TMap pour supprimer les fichiers du disque dur
		for (const TPair<FString, float>& SaveEntry : MasterSave->MySaves)
		{
			UGameplayStatics::DeleteGameInSlot(SaveEntry.Key, 0);
		}

		// On vide complètement le dictionnaire
		MasterSave->MySaves.Empty();

		return UGameplayStatics::SaveGameToSlot(MasterSave, "SaveSlotMySaves", 0);
	}

	return false;
}


// =============================
// =   SerializeObjectToBytes  =
// =============================
TArray<uint8> UHarteosFunctionSave::SerializeObjectToBytes(UObject* ObjectToSave)
{
	// 1. On crée le tableau vide qui va contenir le résultat
	TArray<uint8> OutByteData;

	// 2. Si l'objet est invalide, on retourne le tableau vide
	if (!ObjectToSave) return OutByteData;

	// 3. On remplit le tableau avec la magie de la sérialisation
	FMemoryWriter MemoryWriter(OutByteData);
	FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
	Archive.ArIsSaveGame = true; // On prend UNIQUEMENT les variables cochées "Save Game"
	ObjectToSave->Serialize(Archive);

	// 4. On retourne le tableau rempli
	return OutByteData;
}


// ============================
// =   TriggerGetDataForSave  =
// ============================
TArray<FActorSaveData> UHarteosFunctionSave::TriggerGetDataForSave(const UObject* WorldContextObject)
{
	TArray<FActorSaveData> SerializedActorsList;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return SerializedActorsList;

	// Trouver TOUS les acteurs qui ont ton interface
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, USaveableActor::StaticClass(), FoundActors);

	// Boucler sur les acteurs pour faire leurs valises
	for (AActor* Actor : FoundActors)
	{
		if (!Actor) continue;

		// --- On prévient l'acteur de préparer ses bagages ! ---
		if (Actor->GetClass()->ImplementsInterface(USaveableActor::StaticClass()))
		{
			ISaveableActor::Execute_OnPrepareForSave(Actor);
		}
		// ----------------------------------------------------------------

		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetFName(); // Sa plaque d'immatriculation
		ActorData.Transform = Actor->GetActorTransform(); // Sa position/rotation/échelle

		ActorData.ByteData = SerializeObjectToBytes(Actor);

		// On ajoute cet acteur au grand tableau
		SerializedActorsList.Add(ActorData);
	}

	// On retourne l'array des acteurs trouver
	return SerializedActorsList;
}





// ===============================
// =  DeserializeObjectFromBytes =
// ===============================
void UHarteosFunctionSave::DeserializeObjectFromBytes(UObject* ObjectToLoad, const TArray<uint8>& InByteData)
{
	// Si l'objet n'existe pas ou que la boîte binaire est vide, on ne fait rien
	if (!ObjectToLoad || InByteData.Num() == 0) return;

	FMemoryReader MemoryReader(InByteData);
	FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
	Archive.ArIsSaveGame = true; // On ne remplit QUE les variables cochées "Save Game"

	// L'objet lit l'archive et écrase ses variables actuelles avec les anciennes valeurs !
	ObjectToLoad->Serialize(Archive);
}


// ============================
// =    TriggerLoadActors     =
// ============================
void UHarteosFunctionSave::TriggerLoadActors(const UObject* WorldContextObject, const TArray<FActorSaveData>& SavedActors)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World || SavedActors.Num() == 0) return;

	// 1. On cherche tous les acteurs du niveau qui ont l'interface
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithInterface(WorldContextObject, USaveableActor::StaticClass(), FoundActors);

	// 2. OPTIMISATION : On range les acteurs du niveau dans un dictionnaire (TMap)
	// Clé : Son FName unique / Valeur : Le pointeur vers l'acteur
	TMap<FName, AActor*> ActorLevelMap;
	for (AActor* Actor : FoundActors)
	{
		if (Actor)
		{
			ActorLevelMap.Add(Actor->GetFName(), Actor);
		}
	}

	// 3. On parcourt le tableau des données chargées depuis le fichier
	for (const FActorSaveData& ActorData : SavedActors)
	{
		// Est-ce que cet acteur existait dans le niveau ? On le cherche par son nom.
		if (AActor** FoundActorPtr = ActorLevelMap.Find(ActorData.ActorName))
		{
			AActor* ActorToRestore = *FoundActorPtr;
			if (!ActorToRestore) continue;

			// A. On le téléporte à son ancienne position/rotation/échelle
			// Le type "TeleportPhysics" évite que la physique du moteur pète un câble pendant le déplacement
			ActorToRestore->SetActorTransform(ActorData.Transform, false, nullptr, ETeleportType::TeleportPhysics);

			// B. On réinjecte ses variables spécifiques (vie, inventaire...) depuis sa boîte noire binaire
			DeserializeObjectFromBytes(ActorToRestore, ActorData.ByteData);

			// --- On prévient l'acteur qu'il a retrouvé sa mémoire ! ---
			if (ActorToRestore->GetClass()->ImplementsInterface(USaveableActor::StaticClass()))
			{
				ISaveableActor::Execute_OnLoaded(ActorToRestore);
			}
			// --------------------------------------------------------------------
		}
	}
}