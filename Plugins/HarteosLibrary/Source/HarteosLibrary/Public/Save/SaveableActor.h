// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveableActor.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USaveableActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HARTEOSLIBRARY_API ISaveableActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Appelé juste AVANT que le système aspire les variables (Top Départ)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Harteos Save System")
	void OnPrepareForSave();

	// Appelé juste APRÈS que le système ait réinjecté les variables (Réveil)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Harteos Save System")
	void OnLoaded();
};