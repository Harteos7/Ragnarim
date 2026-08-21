// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameMode_Ragnarim.h"
#include "Kismet/GameplayStatics.h"


//  =================
//  = Lazy accessor =
//  =================
AGameMode_Ragnarim* AGameMode_Ragnarim::GetGameModeRagnarim(const UObject* WorldContextObject)
{
    // On utilise UGameplayStatics pour récupérer la Game Instance du monde actuel
    return Cast<AGameMode_Ragnarim>(UGameplayStatics::GetGameMode(WorldContextObject));
}


//  =============
//  = Construct =
//  =============
AGameMode_Ragnarim::AGameMode_Ragnarim()
{
    PrimaryActorTick.bStartWithTickEnabled = false;
}

// Called every frame
void AGameMode_Ragnarim::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    GameDuration = DeltaTime + GameDuration;


}

