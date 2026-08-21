// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameMode_Ragnarim.generated.h"

/**
 * 
 */
UCLASS()
class RAGNARIM_API AGameMode_Ragnarim : public AGameModeBase
{
	GENERATED_BODY()

public:
    // =============
    // = Functions =
    // =============

    AGameMode_Ragnarim();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Lazy Accessor
    UFUNCTION(BlueprintPure, Category = "Ragnarim|Accessor", meta = (WorldContext = "WorldContextObject"))
    static AGameMode_Ragnarim* GetGameModeRagnarim(const UObject* WorldContextObject);


    // ============
    // = Variable =
    // ============

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Status")
    float GameDuration = 0.0f;

};
