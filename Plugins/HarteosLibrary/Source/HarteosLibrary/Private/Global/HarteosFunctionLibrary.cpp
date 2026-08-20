// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/HarteosFunctionLibrary.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Engine/Engine.h"
#include "Materials/MaterialInstanceDynamic.h"


// Picture


// =====================================================
// =     Take_Picture_From_SceneCaptureComponent	   =
// =====================================================
UMaterialInstanceDynamic* UHarteosFunctionLibrary::Take_Picture_From_SceneCaptureComponent(USceneCaptureComponent2D* SceneComponent, UTextureRenderTarget2D* RenderTarget, FString RenderString, UMaterialInterface* BaseMaterial)
{
	if (!SceneComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Take_Picture_From_Polaroid: SceneComponent est null !"));
		return nullptr;
	}

	if (!RenderTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Take_Picture_From_Polaroid: Aucun RenderTarget n'est assigné au SceneCapture2D !"));
		return nullptr;
	}

	// DUPLIQUER l'asset de référence (copie le fond gris, le gamma, le format)
	UTextureRenderTarget2D* NewRenderTarget = DuplicateObject<UTextureRenderTarget2D>(RenderTarget, GetTransientPackage(), FName(*RenderString));

	if (!NewRenderTarget) return nullptr;

	// S'assurer que la ressource GPU est créée
	NewRenderTarget->UpdateResource();

	SceneComponent->CaptureSource = SCS_FinalColorLDR;
	SceneComponent->MaxViewDistanceOverride = 200.f;
	SceneComponent->bCaptureEveryFrame = false;
	SceneComponent->bCaptureOnMovement = false;

	SceneComponent->TextureTarget = NewRenderTarget;

	// ⬇️ Force le moteur à finir le rendu avant capture
	FlushRenderingCommands();

	SceneComponent->CaptureScene();

	// --- Créer une instance de matériel dynamique à partir d'une texture ---
	if (!BaseMaterial && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BaseMaterial pas passer ou invalide dans Take_Picture_From_SceneCaptureComponent"));
		return nullptr;
	}

	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, nullptr);
	DynMaterial->SetTextureParameterValue("RenderTexture", NewRenderTarget);

	return DynMaterial;
}



















// Tableau


// ===========================
// =   ZoomEventWithBorder   =
// ===========================
FVector2D UHarteosFunctionLibrary::ZoomEventWithBorder(UCanvasPanel* TargetCanvas, FVector2D MouseLocation, FVector2D DefaultTranslation, FVector2D Size, float ZoomStep, float ZoomDelta, float ZoomMax, float ZoomMin)
{
	if (!TargetCanvas && GEngine)
	{
		FString ErrorMessage = FString::Printf(TEXT("ERREUR: TargetCanvas pas valide dans ZoomEventWithBorder ."));

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, ErrorMessage);

		return FVector2D(0, 0);
	}

	FWidgetTransform OldTransformCanvas = TargetCanvas->GetRenderTransform();
	float CurrentScale = OldTransformCanvas.Scale.X;
	float AddedZoom = ZoomDelta * ZoomStep;
	float TargetScale = CurrentScale + AddedZoom;

	// On applique le clamp sur la valeur cible
	float FinalScale = FMath::Clamp(TargetScale, ZoomMin, ZoomMax);

	// Calcule de la bordure
	FVector2D NewMin;

	NewMin.X = (DefaultTranslation.X + ((FinalScale - 1) * Size.X)) * -1;
	NewMin.Y = (DefaultTranslation.Y + ((FinalScale - 1) * Size.Y)) * -1;


	FWidgetTransform NewTransformCanvas = TargetCanvas->GetRenderTransform();
	NewTransformCanvas.Scale = FVector2D(FinalScale, FinalScale);
	FVector2D DecalMouse = MouseLocation * (FVector2D(CurrentScale, CurrentScale) - FVector2D(FinalScale, FinalScale));

	NewTransformCanvas.Translation.X = FMath::Clamp(OldTransformCanvas.Translation.X + DecalMouse.X, NewMin.X, 0);
	NewTransformCanvas.Translation.Y = FMath::Clamp(OldTransformCanvas.Translation.Y + DecalMouse.Y, NewMin.Y, 0);

	TargetCanvas->SetRenderTransform(NewTransformCanvas);

	return NewMin;

}