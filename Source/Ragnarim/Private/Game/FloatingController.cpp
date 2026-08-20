// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FloatingController.h"
#include "Kismet/GameplayStatics.h"


AFloatingController::AFloatingController()
{
	bShowMouseCursor = true; // Ba ouai tu va faire quoi sans dans un jeu de gestion ? boufon va
	DefaultMouseCursor = EMouseCursor::Default;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	SubtitleComp = CreateDefaultSubobject<USubtitleComp>(TEXT("SubtitleSystem"));
}


AFloatingController* AFloatingController::GetFloatingController(const UObject* WorldContextObject)
{
	// On récupère le Controller du joueur 0 et on le Cast en AFloatingController
	return Cast<AFloatingController>(UGameplayStatics::GetPlayerController(WorldContextObject, 0));
}