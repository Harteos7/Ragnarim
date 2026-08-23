#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "MappingLibrary.generated.h"

UCLASS()
class MAPPING_ET_GRAPHIQUE_API UMappingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Mapping")
	static FString WhatTypeKeyboard();

	UFUNCTION(BlueprintCallable, Category = "Mapping")
	static void ApplyDefaultMapping(TArray<UInputMappingContext*> IMC_ARRAY, UInputAction* ModifierActionShift);

	UFUNCTION(BlueprintCallable, Category = "Mapping")
	static void DebugLogMapping();

	// Fonctions utilitaires internes (non Blueprint si pas besoin, ou BlueprintPure)
	static void SaveKeyboardSave(const FString& Layout);
	static FString LoadKeyboardSave();




	UFUNCTION(BlueprintCallable, Category = "Mapping")
	static TArray<FEnhancedActionKeyMapping> GetMappingIMC(UInputMappingContext* IMC);

	UFUNCTION(BlueprintCallable, Category = "Mapping")
	static void SetKeyForActionInIMC(UInputMappingContext* IMC, UInputAction* Action, FKey NewKey, int32 SlotIndex, bool bWantShift, UInputAction* ModifierActionShift);

	UFUNCTION(BlueprintCallable, Category = "Mapping")
	static FInputActionKey GetInputAction_Key(UInputMappingContext* IMC, UInputAction* Action, int32 SlotIndex);

	UFUNCTION(BlueprintPure, Category = "Inputs|Mapping")
	static FText GetActionKeyDisplayName(UInputMappingContext* IMC, UInputAction* Action, int32 SlotIndex = 0);

	UFUNCTION(BlueprintCallable, Category = "Mapping")
	static TArray<FInputActionKey> GetUseKey(FKey SearchKey, bool shift);

	UFUNCTION(BlueprintCallable, Category = "Mapping")
	static void ClearInputAction_Key();

	// Retourne le nom propre d'une touche (corrige l'AZERTY) à partir d'une FKey
	UFUNCTION(BlueprintPure, Category = "Inputs|Mapping")
	static FText GetCleanKeyDisplayName(FKey Key);

	UFUNCTION(BlueprintCallable, Category = "Mapping")
	static float GetSimpleFloatSave(FString NameOptionSave);

	UFUNCTION(BlueprintCallable, Category = "Mapping")
	static void SaveSimpleFloatSave(FString NameOptionSave, float FloatToSave);

	UFUNCTION(BlueprintCallable, Category = "Mapping")
	static UFont* GetFontSave();

	UFUNCTION(BlueprintCallable, Category = "Mapping")
	static void SaveFontSave(UFont* FontToSave);

	UFUNCTION(BlueprintCallable, Category = "Mapping")
	static void UpdateSaveGame(UInputMappingContext* IMC, UInputAction* Action, FKey NewKey, int32 SlotIndex, bool bWantShift);


	
};