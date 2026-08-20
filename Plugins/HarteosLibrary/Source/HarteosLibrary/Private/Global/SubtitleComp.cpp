// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/SubtitleComp.h"
#include "Blueprint/UserWidget.h"
#include "Global/HarteosFunctionLibrary.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Border.h"
#include "Components/RichTextBlock.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

USubtitleComp::USubtitleComp() { PrimaryComponentTick.bCanEverTick = false; }






void USubtitleComp::BeginPlay()
{
	Super::BeginPlay();

	// --- GARDE-FOU ---
	UWorld* World = GetWorld();
	if (!World) return;

	APlayerController* PC = World->GetFirstPlayerController();
	if (PC && SubtitleWidgetClass)
	{
		SubtitleWidgetInstance = CreateWidget<UUserWidget>(PC, SubtitleWidgetClass);
		if (SubtitleWidgetInstance)
		{
			SubtitleWidgetInstance->AddToViewport(1000);
		}
	}
	else if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("SubtitleWidgetClass pas donnée dans le SubtitleComp, echec de ca création"));
	}
}











void USubtitleComp::DisplaySubtitle(FText SubtitleText, float Duration, class USoundBase* AudioEvent)
{
	if (!SubtitleWidgetInstance) return;

	// --- GARDE-FOU ---
	UWorld* World = GetWorld();
	if (!World) return;

	//UE_LOG(LogTemp, Warning, TEXT("Duration demandée : %f"), Duration);

	// --- SÉCURITÉ ANTI-CHEVAUCHEMENT ---
	// Si un son de la ligne précédente joue encore, on le coupe avant de lancer le nouveau
	if (CurrentAudioComponent)
	{
		CurrentAudioComponent->Stop();
		CurrentAudioComponent = nullptr;
	}

	// 1. On annule le chrono précédent manuellement par sécurité
	World->GetTimerManager().ClearTimer(SubtitleTimerHandle);

	// 2. On affiche le nouveau texte
	// Récupérer le RichTextBlock nommé "SousTitre"
	URichTextBlock* RichText = Cast<URichTextBlock>(SubtitleWidgetInstance->GetWidgetFromName(TEXT("SousTitre")));
	if (!RichText && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Block SousTitre introuvable dans le widget"));
		return;
	}

	// Mettre à jour le texte
	RichText->SetText(SubtitleText);

	// Rendre visible si besoin
	RichText->SetVisibility(ESlateVisibility::Visible);

	// Récupérer le Border nommé "Border"
	UBorder* Border = Cast<UBorder>(SubtitleWidgetInstance->GetWidgetFromName(TEXT("Border")));
	if (!Border && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Block Border introuvable dans le widget"));
		return;
	}

	// Rendre le fond visible
	Border->SetVisibility(ESlateVisibility::Visible);

	// 3. Gestion de l'audio natif
	if (AudioEvent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tentative de lecture du son : %s"), *AudioEvent->GetName());
		// SpawnSound2D lance le son et retourne le composant pour qu'on puisse l'arrêter plus tard
		CurrentAudioComponent = UGameplayStatics::SpawnSound2D(World, AudioEvent, DialogueVolumeMultiplier);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Pas d'AudioEvent valide recu pour ce sous-titre !"));
		CurrentAudioComponent = nullptr;
	}

	float FinalDuration = Duration;

	// 4. Calcul de la durée VIA l'instance si y'a de l'audio et que la duration est de 0
	if (FinalDuration <= 0.0f && AudioEvent != nullptr)
	{
		FinalDuration = AudioEvent->GetDuration();
	}

	// 5. Garde-fou : si la durée est toujours à 0 (ex: audio très court ou erreur), on met une valeur par défaut bien spécial
	if (FinalDuration <= 0.0f)
	{
		FinalDuration = 2.123456f;
	}

	UE_LOG(LogTemp, Warning, TEXT("Duration finale appliquee : %f"), FinalDuration);

	// 6. Lancement du chrono avec la durée finale
	World->GetTimerManager().SetTimer(SubtitleTimerHandle, this, &USubtitleComp::DisplayNextInQueue, FinalDuration, false);
}









bool USubtitleComp::IsSubtitleOnScreen() const
{
	if (!SubtitleWidgetInstance) return false;

	// Récupérer le RichTextBlock nommé "SousTitre"
	URichTextBlock* RichText = Cast<URichTextBlock>(SubtitleWidgetInstance->GetWidgetFromName(TEXT("SousTitre")));
	if (!RichText && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Block SousTitre introuvable dans le widget"));
		return false;
	}

	if (!RichText->IsVisible())
	{
		return false;
	}

	if (RichText->GetText().IsEmpty())
	{
		return false;
	}

	return true;
}






void USubtitleComp::EndSubtitleTimer()
{
	// Arrêt de l'audio natif
	if (CurrentAudioComponent != nullptr)
	{
		CurrentAudioComponent->Stop();
		CurrentAudioComponent = nullptr;
	}

	if (!SubtitleWidgetInstance) return;

	// Récupérer le RichTextBlock nommé "SousTitre"
	URichTextBlock* RichText = Cast<URichTextBlock>(SubtitleWidgetInstance->GetWidgetFromName(TEXT("SousTitre")));
	if (!RichText && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Block SousTitre introuvable dans le widget"));
		return;
	}

	// Mettre à jour le texte
	RichText->SetText(FText::FromString(""));

	// Récupérer le Border nommé "Border"
	UBorder* Border = Cast<UBorder>(SubtitleWidgetInstance->GetWidgetFromName(TEXT("Border")));
	if (!Border && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Block Border introuvable dans le widget"));
		return;
	}

	// Masquer et effacer l'espace du fond noir
	Border->SetVisibility(ESlateVisibility::Collapsed);
}







void USubtitleComp::PlaySubtitleSequence(TArray<FSubtitleLine> Sequence, FName SequenceID)
{
	if (Sequence.Num() == 0) return;

	// On stocke l'ID pour s'en souvenir à la fin
	CurrentSequenceID = SequenceID;

	// On remplit la file d'attente et on repart de zéro
	SubtitleQueue = Sequence;
	CurrentQueueIndex = 0;

	DisplayNextInQueue();
}



void USubtitleComp::DisplayNextInQueue()
{
	if (SubtitleQueue.IsValidIndex(CurrentQueueIndex))
	{
		// On récupère la ligne actuelle
		FSubtitleLine& CurrentLine = SubtitleQueue[CurrentQueueIndex];

		// On utilise ta fonction existante pour l'affichage physique et elle se charge du calcul durer FMOD
		DisplaySubtitle(CurrentLine.Text, CurrentLine.Duration, CurrentLine.Sound);

		// On prépare l'index pour la prochaine fois
		CurrentQueueIndex++;
	}
	else
	{
		// Plus rien dans la liste ? On nettoie le widget
		EndSubtitleTimer();

		// ON PRÉVIENT LE BLUEPRINT :
		OnSequenceFinished.Broadcast(CurrentSequenceID);

		CurrentSequenceID = NAME_None;
	}
}










void USubtitleComp::SkipCurrentSubtitle()
{
	// --- GARDE-FOU ---
	UWorld* World = GetWorld();
	if (!World) return;

	// On vérifie s'il y a un timer en cours (donc un sous-titre affiché)
	if (World->GetTimerManager().IsTimerActive(SubtitleTimerHandle))
	{
		// On stoppe le timer immédiatement
		World->GetTimerManager().ClearTimer(SubtitleTimerHandle);

		// On stoppe l'audio
		EndSubtitleTimer();

		// On appelle la fonction pour passer au suivant
		DisplayNextInQueue();
	}
}











void USubtitleComp::PlayDialogueFromTable(FName RowName, FName SequenceID)
{
	if (!MainSubtitleTable)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Table manquante"));
		}
		return;
	}

	if (RowName == NAME_None)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("RowName invalide ! switch sur default"));
		}
		RowName = FName("Default");
	}

	static const FString ContextString(TEXT("Recherche Sequence Dialogue Global"));
	FSubtitleArray* FoundRow = MainSubtitleTable->FindRow<FSubtitleArray>(RowName, ContextString);

	if (!FoundRow && RowName != FName("Default"))
	{
		if (GEngine)
		{
			FString ErrorMsg = FString::Printf(TEXT("Ligne '%s' introuvable ! Repli sur default"), *RowName.ToString());
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, ErrorMsg);
		}

		// On force le nom à "Default" et on refait la recherche
		RowName = FName("Default");
		FoundRow = MainSubtitleTable->FindRow<FSubtitleArray>(RowName, ContextString);
	}



	if (FoundRow)
	{
		TArray<FSubtitleLine> GeneratedSequence;

		// --- OPTIMISATION : ON PRÉPARE LES ARGUMENTS UNE SEULE FOIS ICI ---
		FFormatNamedArguments Args;

		// On récupère les textes des touches
		//FText ToucheLampeUV = UMappingLibrary::GetActionKeyDisplayName(MyIMCAction, ActionLampUV, 0);
		//FText TouchePolaroid = UMappingLibrary::GetActionKeyDisplayName(MyIMCAction, ActionPolaroid, 0);
		//FText ToucheTimeTravel = UMappingLibrary::GetActionKeyDisplayName(MyIMCAction, ActionTimeTravel, 0);

		//Args.Add(TEXT("LampUV"), ToucheLampeUV);
		//Args.Add(TEXT("Polaroid"), TouchePolaroid);
		//Args.Add(TEXT("TimeTravel"), ToucheTimeTravel);

		// On boucle sur le nombre de textes présents dans cette ligne
		for (int32 i = 0; i < FoundRow->Text.Num(); ++i)
		{
			FSubtitleLine TempLine;
			TempLine.Text = FoundRow->Text[i];

			// Unreal va prendre le texte de la table et remplacer automatiquement {variable} par la touche correspondante
			TempLine.Text = FText::Format(FoundRow->Text[i], Args);

			// GARDE-FOU : Vérification index des durées
			TempLine.Duration = FoundRow->Duration.IsValidIndex(i) ? FoundRow->Duration[i] : 0.0f;

			// GARDE-FOU : Vérification index de l'audio
			TempLine.Sound = FoundRow->Sound.IsValidIndex(i) ? FoundRow->Sound[i] : nullptr;

			GeneratedSequence.Add(TempLine);
		}

		if (GeneratedSequence.Num() > 0)
		{
			PlaySubtitleSequence(GeneratedSequence, SequenceID);
		}
	}
	else
	{
		// Sécurité absolue : Si même la ligne "Default" n'existe pas dans ta DataTable
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("ERREUR CRITIQUE : Même la ligne 'Default' n'existe pas !"));
		}
	}
}