// Fill out your copyright notice in the Description page of Project Settings.


#include "GraphiqueLibrary.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/ComboBoxString.h"
#include "Blueprint/UserWidget.h"
#include "Math/IntPoint.h"
#include "Misc/Parse.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/GameUserSettings.h"


void UGraphiqueLibrary::MessageTest(FString Message)
{
	UKismetSystemLibrary::PrintString(nullptr, Message, true, true, FColor::Green, 2.0f);
}





// ==========================================
// =     FONCTION UNIQUE : CHANGEMENT       =
// ==========================================
void UGraphiqueLibrary::ChangerQualiteGraphique(EQualityType Type, int32 Qualite)
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (!Settings) return;

	switch (Type)
	{
	case EQualityType::Texture:            Settings->SetTextureQuality(Qualite); break;
	case EQualityType::Shadow:             Settings->SetShadowQuality(Qualite); break;
	case EQualityType::VisualEffect:       Settings->SetVisualEffectQuality(Qualite); break;
	case EQualityType::Foliage:            Settings->SetFoliageQuality(Qualite); break;
	case EQualityType::ViewDistance:       Settings->SetViewDistanceQuality(Qualite); break;
	case EQualityType::PostProcessing:     Settings->SetPostProcessingQuality(Qualite); break;
	case EQualityType::GlobalIllumination: Settings->SetGlobalIlluminationQuality(Qualite); break;
	case EQualityType::Reflection:         Settings->SetReflectionQuality(Qualite); break;
	case EQualityType::Shading:            Settings->SetShadingQuality(Qualite); break;
	case EQualityType::GlobalValue:		   Settings->SetOverallScalabilityLevel(Qualite); break;
	}

	Settings->ApplySettings(false);
	Settings->SaveSettings();
}







// =========================================
// =     FONCTION UNIQUE : Get Value       =
// =========================================
int UGraphiqueLibrary::GetQualityTypeValue(EQualityType Type)
{
	int32 QualityValue = 0;

	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (!Settings) return QualityValue;

	switch (Type)
	{
	case EQualityType::Texture:            QualityValue = Settings->GetTextureQuality(); break;
	case EQualityType::Shadow:             QualityValue = Settings->GetShadowQuality(); break;
	case EQualityType::VisualEffect:       QualityValue = Settings->GetVisualEffectQuality(); break;
	case EQualityType::Foliage:            QualityValue = Settings->GetFoliageQuality(); break;
	case EQualityType::ViewDistance:       QualityValue = Settings->GetViewDistanceQuality(); break;
	case EQualityType::PostProcessing:     QualityValue = Settings->GetPostProcessingQuality(); break;
	case EQualityType::GlobalIllumination: QualityValue = Settings->GetGlobalIlluminationQuality(); break;
	case EQualityType::Reflection:         QualityValue = Settings->GetReflectionQuality(); break;
	case EQualityType::Shading:            QualityValue = Settings->GetShadingQuality(); break;
	case EQualityType::GlobalValue:		   QualityValue = Settings->GetOverallScalabilityLevel(); break;
	}

	return QualityValue;
}














void UGraphiqueLibrary::ChangerQualiteGlobal(int32 Qualite)
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (Settings)
	{
		Settings->SetOverallScalabilityLevel(Qualite);
		Settings->ApplySettings(false); // le false c'est pour ne pas redémarer lors de l'application des settings
		Settings->SaveSettings();
	}
}

void UGraphiqueLibrary::ChangerAntiAliasing(int32 Index)
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (Settings)
	{
		Settings->SetAntiAliasingQuality(Index);
		Settings->ApplySettings(false);
	}
	else
	{
		UKismetSystemLibrary::PrintString(nullptr, "Erreur Index", true, true, FColor::Red, 2.0f);
	}
}

void UGraphiqueLibrary::SetComboStringAntiAliasing(UComboBoxString* ComboString)
{
	if (!ComboString)
	{
		UE_LOG(LogTemp, Error, TEXT("SetComboStringAntiAliasing : Le ComboBox fourni est NULL !"));
		return;
	}

	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (!Settings) return;

	ComboString->ClearOptions(); // Nettoyage au cas où
	ComboString->AddOption(TEXT("Off"));  // Index 0
	ComboString->AddOption(TEXT("FXAA")); // Index 1
	ComboString->AddOption(TEXT("TAA"));  // Index 2
	ComboString->AddOption(TEXT("MSAA")); // Index 3
	ComboString->AddOption(TEXT("TSR"));  // Index 4

	int32 Quality = Settings->GetAntiAliasingQuality();
	ComboString->SetSelectedIndex(Quality);
}


void UGraphiqueLibrary::GetVsync(UTextBlock* TextBlock)
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (Settings && TextBlock)
	{
		bool Vsync = Settings->IsVSyncEnabled();

		if (Vsync)
		{
			TextBlock->SetText(FText::FromString("Vsync : On"));
		}
		else
		{
			TextBlock->SetText(FText::FromString("Vsync : Off"));
		}
	}
	else
	{
		UKismetSystemLibrary::PrintString(nullptr, "TextBlock mal définie", true, true, FColor::Red, 2.0f);
	}
}

void UGraphiqueLibrary::ChangerVsync(UTextBlock* TextBlock)
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (Settings && TextBlock)
	{
		bool Vsync = Settings->IsVSyncEnabled();

		if (Vsync)
		{
			TextBlock->SetText(FText::FromString("Vsync : Off"));
			Settings->SetVSyncEnabled(false);
			Settings->ApplySettings(false);
		}
		else
		{
			TextBlock->SetText(FText::FromString("Vsync : On"));
			Settings->SetVSyncEnabled(true);
			Settings->ApplySettings(false);
		}
	}
	else
	{
		UKismetSystemLibrary::PrintString(nullptr, "TextBlock mal définie", true, true, FColor::Red, 2.0f);
	}
}


void UGraphiqueLibrary::ScrollResolution(float Value, FIntPoint ResolutionChoose)
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (Settings)
	{
		Value = Value / 100; // Pour mettre en facteur 0 à 1
		int ResolutionChooseX = FMath::TruncToInt(ResolutionChoose.X * Value);
		int ResolutionChooseY = FMath::TruncToInt(ResolutionChoose.Y * Value);

		FIntPoint Resolution = FIntPoint(ResolutionChooseX, ResolutionChooseY);
		Settings->SetScreenResolution(Resolution);
		Settings->ApplySettings(false);
		Settings->SaveSettings();
	}
	else
	{
		UKismetSystemLibrary::PrintString(nullptr, "Erreur dans settings", true, true, FColor::Red, 2.0f);
	}
}

void UGraphiqueLibrary::ResolutionAffichageButton(EWindowMode::Type WindowMode, FIntPoint ResolutionChoose)
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (Settings)
	{
		Settings->SetFullscreenMode(WindowMode);
		Settings->SetScreenResolution(ResolutionChoose);
		Settings->ApplySettings(false);
		Settings->SaveSettings();
	}
	else
	{
		UKismetSystemLibrary::PrintString(nullptr, "Erreur dans settings", true, true, FColor::Red, 2.0f);
	}
}

void UGraphiqueLibrary::SetFPS(float FPS)
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (Settings)
	{
		Settings->SetFrameRateLimit(FPS);
		Settings->ApplySettings(false);
		Settings->SaveSettings();
	}
	else
	{
		UKismetSystemLibrary::PrintString(nullptr, "Erreur dans settings", true, true, FColor::Red, 2.0f);
	}
}


TArray<FIntPoint> UGraphiqueLibrary::InitResolution()
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	TArray<FIntPoint> Resolutions;
	if (Settings)
	{
		UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);
		FIntPoint ResolutionSreen = Settings->GetScreenResolution();
	}
	else
	{
		UKismetSystemLibrary::PrintString(nullptr, "Erreur dans settings", true, true, FColor::Red, 2.0f);
	}
	return Resolutions;
}

FIntPoint UGraphiqueLibrary::CurrentResolution()
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (Settings)
	{
		FIntPoint ResolutionSreen = Settings->GetScreenResolution();
		return ResolutionSreen;
	}
	else
	{
		UKismetSystemLibrary::PrintString(nullptr, "Erreur dans settings", true, true, FColor::Red, 2.0f);
		return FIntPoint(0, 0);
	}
}

TArray<FIntPoint> UGraphiqueLibrary::EventConstructResolutions(TArray<FIntPoint> Resolutions, UComboBoxString* ComboString, FIntPoint CurrentResolution)
{
	bool CurrentResolutionBool = false;

	for (const auto& intpoint : Resolutions)
	{
		ComboString->AddOption(intpoint.ToString());
		if (intpoint == CurrentResolution)
		{
			CurrentResolutionBool = true;
			//UKismetSystemLibrary::PrintString(
			//	nullptr,
			//	FString::Printf(TEXT("CurrentResolution trouver : X=%d, Y=%d"),
			//		CurrentResolution.X, CurrentResolution.Y),
			//	true, true, FColor::Green, 2.0f);
		}
	}

	if (!CurrentResolutionBool)
	{
		Resolutions.Add(CurrentResolution);
		ComboString->AddOption(CurrentResolution.ToString());
	}

	ComboString->SetSelectedOption(CurrentResolution.ToString());

	//for (const auto& intpoint : Resolutions)
	//{
	//	UKismetSystemLibrary::PrintString(
	//		nullptr,
	//		FString::Printf(TEXT("Affichange des valeurs | Resolution: %d x %d"),
	//			intpoint.X, intpoint.Y),
	//		true, true, FColor::Green, 200.0f);
	//}

	return Resolutions;
}

FIntPoint UGraphiqueLibrary::ComboboxChangeResolution(FString Value)
{
	FIntPoint CurrentResolution(0, 0);

	// Extraction des valeurs "X=" et "Y="
	if (FParse::Value(*Value, TEXT("X="), CurrentResolution.X) && FParse::Value(*Value, TEXT("Y="), CurrentResolution.Y))
	{
		UE_LOG(LogTemp, Log, TEXT("Résolution extraite : %d x %d"), CurrentResolution.X, CurrentResolution.Y);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Format de résolution invalide : %s"), *Value);
	}

	return CurrentResolution;
}


