// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FloatingController_RagnaRim.h"



AFloatingController_RagnaRim::AFloatingController_RagnaRim()
{
	bShowMouseCursor = true; // Ba ouai tu va faire quoi sans dans un jeu de gestion ? boufon va
	DefaultMouseCursor = EMouseCursor::Default;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}
