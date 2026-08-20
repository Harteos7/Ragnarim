// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/SceneCapture2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "HarteosFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class HARTEOSLIBRARY_API UHarteosFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// Prendre en photo
	UFUNCTION(BlueprintCallable, Category = "Polaroid")
	static UMaterialInstanceDynamic* Take_Picture_From_SceneCaptureComponent(USceneCaptureComponent2D* SceneComponent, UTextureRenderTarget2D* RenderTarget, FString RenderString, UMaterialInterface* BaseMaterial);


	// Zoom (canvas et image)
	UFUNCTION(BlueprintCallable, Category = "Zoom")
	static FVector2D ZoomEventWithBorder(UCanvasPanel* TargetCanvas, FVector2D MouseLocation, FVector2D DefaultTranslation, FVector2D Size, float ZoomStep, float ZoomDelta, float ZoomMax, float ZoomMin);
};
