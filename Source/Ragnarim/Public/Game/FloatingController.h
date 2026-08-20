// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Global/SubtitleComp.h"
#include "FloatingController.generated.h"

/**
 * 
 */
UCLASS()
class RAGNARIM_API AFloatingController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// Sets default values for this pawn's properties
	AFloatingController();

	// lazy accessor
	UFUNCTION(BlueprintPure, Category = "Ragnarim|Accessor", meta = (WorldContext = "WorldContextObject"))
	static AFloatingController* GetFloatingController(const UObject* WorldContextObject);

protected:
	// =============
	// = Variables =
	// =============
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USubtitleComp* SubtitleComp;


	// Pawn
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pawn")
	TArray<APawn*> MyPawn;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pawn")
	APawn* SelectedPawn;
};
