// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "FloatingCamera_Ragnarim.generated.h"

UCLASS()
class RAGNARIM_API AFloatingCamera_Ragnarim : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AFloatingCamera_Ragnarim();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// =============
	// = Functions =
	// =============

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;





	// =============
	// = Variables =
	// =============

	// Float pour la vitesse de déplacement de la camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SettingCamera")
	float DeplacementSpeed = 1.0;

	// Float pour la vitesse de déplacement de la camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SettingCamera")
	float RotationSpeed = 1.0;

	// Float pour la vitesse de déplacement de la camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SettingCamera")
	float ZoomSpeed = 100.0;

	// Valeur initial du zoom
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SettingCamera")
	float ZoomInitalValue = 700;

	// Valeur initial du zoom
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SettingCamera")
	float EdgeScrollSpeed = 1.0;






	// =============
	// = Component =
	// =============

	// UFloatingPawnMovement pour gérer les déplacements d'un pown qui n'a pas de graviter/sol sous les pieds
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UFloatingPawnMovement* MovementComponent;

	// Sous ton MovementComponent
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class UCameraComponent* Camera;


};
