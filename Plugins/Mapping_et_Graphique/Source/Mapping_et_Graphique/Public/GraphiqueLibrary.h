// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GraphiqueLibrary.generated.h"

// Déclaration de l'ENUM pour centraliser tous les types de paramètres graphiques
UENUM(BlueprintType)
enum class EQualityType : uint8
{
	Texture,
	Shadow,
	VisualEffect,
	Foliage,
	ViewDistance,
	PostProcessing,
	GlobalIllumination,
	Reflection,
	Shading,
	GlobalValue
};



UCLASS()
class MAPPING_ET_GRAPHIQUE_API UGraphiqueLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Menu Graphique test")
	static void MessageTest(FString Message);




	// --- LES DEUX FONCTIONS UNIQUES ET UNIFIÉES ---
	UFUNCTION(BlueprintCallable, Category = "Options Graphiques")
	static void ChangerQualiteGraphique(EQualityType Type, int32 Qualite);

	UFUNCTION(BlueprintCallable, Category = "Options Graphiques")
	static int32 GetQualityTypeValue(EQualityType Type);





	UFUNCTION(BlueprintCallable, Category = "Options Graphiques")
	static void ChangerQualiteGlobal(int32 Qualite);


	UFUNCTION(BlueprintCallable, Category = "Options Graphiques")
	static void ChangerAntiAliasing(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Options Graphiques")
	static void SetComboStringAntiAliasing(UComboBoxString* ComboString);


	UFUNCTION(BlueprintCallable, Category = "Options Graphiques")
	static FString ChangerVsync();

	UFUNCTION(BlueprintCallable, Category = "Options Graphiques")
	static FString GetVsync();


	UFUNCTION(BlueprintCallable, Category = "Options Graphiques")
	static void ScrollResolution(float Value, FIntPoint ResolutionChoose);

	UFUNCTION(BlueprintCallable, Category = "Options Graphiques")
	static void ResolutionAffichageButton(EWindowMode::Type WindowMode, FIntPoint ResolutionChoose);

	UFUNCTION(BlueprintCallable, Category = "Options Graphiques")
	static void SetFPS(float FPS);




	UFUNCTION(BlueprintCallable, Category = "Options Graphiques")
	static TArray<FIntPoint> InitResolution();

	UFUNCTION(BlueprintCallable, Category = "Options Graphiques")
	static FIntPoint CurrentResolution();

	UFUNCTION(BlueprintCallable, Category = "Options Graphiques")
	static TArray<FIntPoint> EventConstructResolutions(TArray<FIntPoint> Resolutions, UComboBoxString* ComboString, FIntPoint CurrentResolution);

	UFUNCTION(BlueprintCallable, Category = "Options Graphiques")
	static FIntPoint ComboboxChangeResolution(FString Value);

};
