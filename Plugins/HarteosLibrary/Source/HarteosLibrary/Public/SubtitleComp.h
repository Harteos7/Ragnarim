// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MappingLibrary.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Engine/DataTable.h"
#include "SubtitleComp.generated.h"

// ============
// = DELEGATE =
// ============

// On ajoute un paramètre FName au délégate pour savoir qui à lancer et qui doit écouter
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSubtitleSequenceFinished, FName, SequenceID);


// =============
// = Structure =
// =============

// Structure pour les subtitle
USTRUCT(BlueprintType)
struct FSubtitleArray : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitle")
	TArray<FText> Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitle")
	TArray<float> Duration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitle")
	TArray<class USoundBase*> Sound;
};



USTRUCT(BlueprintType)
struct FSubtitleLine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitle")
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitle")
	float Duration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitle")
	class USoundBase* Sound;
};







UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HARTEOSLIBRARY_API USubtitleComp : public UActorComponent
{
	GENERATED_BODY()

public:
	USubtitleComp();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Subtitle")
	class UDataTable* MainSubtitleTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Subtitle")
	TSubclassOf<class UUserWidget> SubtitleWidgetClass;

	// Instance créée
	UPROPERTY(BlueprintReadWrite, Category = "Subtitle")
	class UUserWidget* SubtitleWidgetInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Subtitle")
	FTimerHandle SubtitleTimerHandle;

	TArray<FSubtitleLine> SubtitleQueue;
	int32 CurrentQueueIndex;
	FName CurrentSequenceID;

	// Pour garder une ref de l'audio en cours
	UPROPERTY()
	class UAudioComponent* CurrentAudioComponent;

	// Valeur de configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitle|Audio")
	float DialogueVolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitle|Audio")
	float MusicVolumeMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Subtitle|Inputs")
	UInputMappingContext* MyIMCAction;

	UPROPERTY(EditDefaultsOnly, Category = "Subtitle|Inputs")
	UInputAction* ActionLampUV;

	UPROPERTY(EditDefaultsOnly, Category = "Subtitle|Inputs")
	UInputAction* ActionPolaroid;

	UPROPERTY(EditDefaultsOnly, Category = "Subtitle|Inputs")
	UInputAction* ActionTimeTravel;

public:
	UFUNCTION(BlueprintCallable, Category = "Subtitle")
	void DisplaySubtitle(FText SubtitleText, float Duration, class USoundBase* AudioEvent);

	UFUNCTION(BlueprintPure, Category = "Subtitle")
	bool IsSubtitleOnScreen() const;

	void EndSubtitleTimer();

	UFUNCTION(BlueprintCallable, Category = "Subtitle")
	void PlaySubtitleSequence(TArray<FSubtitleLine> Sequence, FName SequenceID);

	// Fonction interne pour afficher l'élément actuel de la file
	void DisplayNextInQueue();

	/** Passe au sous-titre suivant immédiatement */
	UFUNCTION(BlueprintCallable, Category = "Subtitle")
	void SkipCurrentSubtitle();

	UFUNCTION(BlueprintCallable, Category = "Subtitle")
	void PlayDialogueFromTable(FName RowName, FName SequenceID);



	UPROPERTY(BlueprintAssignable, Category = "Subtitle")
	FOnSubtitleSequenceFinished OnSequenceFinished;
};
