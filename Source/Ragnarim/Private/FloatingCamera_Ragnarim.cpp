// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingCamera_Ragnarim.h"

// Sets default values
AFloatingCamera_Ragnarim::AFloatingCamera_Ragnarim()
{
	bUseControllerRotationYaw = false; // Le Pawn ne suis pas le controller

	// Créer une racine (souvent une SceneComponent pour une caméra)
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Créer le bras
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = ZoomInitalValue; // Distance par défaut
	SpringArm->bUsePawnControlRotation = false; // Le bras suit l'Acteur, pas le controller
	SpringArm->bInheritYaw = true; // Il doit suivre quand le Pawn tourne

	// Créer la caméra et l'attacher au bras
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// Création du composant de mouvement
	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));

	// Valeurs par défaut
	MovementComponent->MaxSpeed = 3000.f;
	MovementComponent->Acceleration = 1500.f;
	MovementComponent->Deceleration = 8000.f;
}

// Called when the game starts or when spawned
void AFloatingCamera_Ragnarim::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFloatingCamera_Ragnarim::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFloatingCamera_Ragnarim::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

