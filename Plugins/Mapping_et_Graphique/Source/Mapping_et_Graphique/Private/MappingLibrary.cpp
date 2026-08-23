#include "MappingLibrary.h"
#include "KeyBoardSave.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputTriggers.h"
#include "Engine/Engine.h"
#include "InputAction.h"
#if PLATFORM_WINDOWS
#include "Windows/WindowsApplication.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif
#if PLATFORM_LINUX
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#endif
#if PLATFORM_MAC
#include <CoreFoundation/CoreFoundation.h>
#include <Carbon/Carbon.h>
#endif


// =============================
// =     WhatTypeKeyboard	   =
// =============================
FString UMappingLibrary::WhatTypeKeyboard()
{
#if PLATFORM_WINDOWS
    // Partie Windows
    HKL KeyboardLayout = GetKeyboardLayout(0);
    int LangId = LOWORD(KeyboardLayout);

    switch (LangId)
    {
    case 0x040C: // France
    case 0x080C: // Belgique
    case 0x0C0C: // Canada (Certains utilisent l'AZERTY ou le QWERTY Canadien)
    case 0x140C: // Luxembourg
        return TEXT("fr-FR");

    case 0x0409: // US
    case 0x0809: // UK
        return TEXT("en-US");

    default: return FString::Printf(TEXT("Unknown (0x%04X)"), LangId);
    }

#elif PLATFORM_LINUX
    Display* display = XOpenDisplay(nullptr);
    if (!display) return TEXT("Unknown-Linux");

    XkbDescRec* xkb = XkbAllocKeyboard();
    if (XkbGetNames(display, XkbGroupNamesMask, xkb) == Success) {
        Atom* groups = xkb->names->groups;
        if (groups) {
            char* name = XGetAtomName(display, groups[0]);
            if (name) {
                FString Layout(ANSI_TO_TCHAR(name));
                XFree(name);
                XkbFreeKeyboard(xkb, 0, True);
                XCloseDisplay(display);

                // Normalisation
                if (Layout == TEXT("fr")) return TEXT("fr-FR");
                if (Layout == TEXT("us")) return TEXT("en-US");
                if (Layout == TEXT("gb")) return TEXT("en-GB");

                return FString::Printf(TEXT("Unknown (%s)"), *Layout);
            }
        }
    }

    XkbFreeKeyboard(xkb, 0, True);
    XCloseDisplay(display);
    return TEXT("Unknown-Linux");
#elif PLATFORM_MAC
    TISInputSourceRef source = TISCopyCurrentKeyboardInputSource();
    if (source) {
        CFStringRef layoutID = (CFStringRef)TISGetInputSourceProperty(source, kTISPropertyInputSourceID);
        if (layoutID) {
            char buffer[256];
            if (CFStringGetCString(layoutID, buffer, sizeof(buffer), kCFStringEncodingUTF8)) {
                FString Layout(buffer);

                if (Layout.Contains(TEXT("French"))) return TEXT("fr-FR");
                if (Layout.Contains(TEXT("US"))) return TEXT("en-US");
                if (Layout.Contains(TEXT("British"))) return TEXT("en-GB");

                return FString::Printf(TEXT("Unknown (%s)"), *Layout);
            }
        }
        CFRelease(source);
    }
    return TEXT("Unknown-Mac");
#elif PLATFORM_XBOXONE || PLATFORM_XBOX || PLATFORM_XBOX_SCARLETT
    // Xbox
    return TEXT("Xbox");
#elif PLATFORM_PS4 || PLATFORM_PS5
    // PlayStation
    return TEXT("PlayStation");
#elif PLATFORM_SWITCH
    // Nintendo Switch
    return TEXT("Switch");
#else
    return TEXT("Unknown platform");
#endif
}


void UMappingLibrary::ApplyDefaultMapping(TArray<UInputMappingContext*> IMC_ARRAY, UInputAction* ModifierActionShift)
{
    if (IMC_ARRAY.IsEmpty()) return;

    UKeyBoardSave* LoadedSAVE = Cast<UKeyBoardSave>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSettings"), 0));
    if (!LoadedSAVE)
    {
        LoadedSAVE = Cast<UKeyBoardSave>(UGameplayStatics::CreateSaveGameObject(UKeyBoardSave::StaticClass()));

        for (UInputMappingContext* IMC : IMC_ARRAY)
        {
            // IMPORTANT : On remplit la save avec les touches ACTUELLES de l'IMC 
            // avant de faire le remapping, sinon SaveInputAction_Key est vide !
            FString IMCName = IMC->GetName();
            TMap<FName, int32> ActionCounters;

            for (const FEnhancedActionKeyMapping& Mapping : IMC->GetMappings())
            {
                if (Mapping.Action)
                {
                    FName Name = Mapping.Action->GetFName();
                    FInputActionKey NewEntry;
                    NewEntry.SaveIMC = IMCName;
                    NewEntry.ActionName = Name;
                    NewEntry.SaveKey = Mapping.Key;
                    NewEntry.bHasShiftModifier = false;

                    for (UInputTrigger* Trigger : Mapping.Triggers)
                    {
                        if (UInputTriggerChordAction* ChordTrigger = Cast<UInputTriggerChordAction>(Trigger))
                        {
                            if (ChordTrigger->ChordAction)
                            {
                                // Vérification du nom de l'action Shift
                                if (ChordTrigger->ChordAction == ModifierActionShift)
                                {
                                    NewEntry.bHasShiftModifier = true;
                                }
                            }
                        }
                    }


                    // Gestion intelligente des slots pour les touches multiples (S / Down etc.)
                    int32& Count = ActionCounters.FindOrAdd(Name);
                    NewEntry.SlotIndex = Count;
                    Count++;

                    LoadedSAVE->SaveInputAction_Key.Add(NewEntry);
                }
            }
        }

        // On initialise le layout à la langue de développemnt
        LoadedSAVE->KeyboardLayout = TEXT("fr-FR");
        UGameplayStatics::SaveGameToSlot(LoadedSAVE, TEXT("PlayerSettings"), 0);
    }

    FString CurrentCulture = WhatTypeKeyboard();
    FString SavedCulture = LoadedSAVE->KeyboardLayout;

    // --- PARTIE 1 : REMAPPING AUTO (AZERTY/QWERTY) ---
    if (CurrentCulture != SavedCulture)
    {
        TMap<FKey, FKey> RemapTable;
        if (CurrentCulture == "fr-FR") {
            RemapTable = {{EKeys::W, EKeys::Z}, {EKeys::Z, EKeys::W}, {EKeys::Q, EKeys::A}, {EKeys::A, EKeys::Q}, {EKeys::M, EKeys::Semicolon}, {EKeys::Semicolon, EKeys::M}};
            //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("on passe en ZQSD "));
        }
        else {
            RemapTable = {{EKeys::Z, EKeys::W}, {EKeys::W, EKeys::Z}, {EKeys::A, EKeys::Q}, {EKeys::Q, EKeys::A}, {EKeys::Semicolon, EKeys::M}, {EKeys::M, EKeys::Semicolon}};
            //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("on passe en WASD"));
        }

        for (FInputActionKey& SaveData : LoadedSAVE->SaveInputAction_Key)
        {
            if (RemapTable.Contains(SaveData.SaveKey))
                SaveData.SaveKey = RemapTable[SaveData.SaveKey];
        }

        LoadedSAVE->KeyboardLayout = CurrentCulture;
        UGameplayStatics::SaveGameToSlot(LoadedSAVE, TEXT("PlayerSettings"), 0);
    }



    // --- PARTIE 2 : APPLICATION SUR L'IMC ---

    for (UInputMappingContext* IMC : IMC_ARRAY)
    {
        FString IMCName = IMC->GetName();

        // 1. On fait une copie propre des mappings
        TArray<FEnhancedActionKeyMapping> CopiedMappings = IMC->GetMappings();
        bool bNeedsUpdate = false;

        for (const FInputActionKey& SaveData : LoadedSAVE->SaveInputAction_Key)
        {
            if (SaveData.SaveIMC != IMCName) continue;

            int32 CurrentFoundIndex = 0;
            // 2. On modifie notre copie en RAM
            for (FEnhancedActionKeyMapping& Mapping : CopiedMappings)
            {
                if (Mapping.Action && Mapping.Action->GetFName() == SaveData.ActionName)
                {
                    if (CurrentFoundIndex == SaveData.SlotIndex)
                    {
                        Mapping.Key = SaveData.SaveKey;
                        bNeedsUpdate = true;
                        //GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan,
                        //    FString::Printf(TEXT("Key Changer : %s pour : %s"),
                        //        *SaveData.ActionName.ToString(),
                        //        *SaveData.SaveKey.ToString()));
                        break;
                    }
                    CurrentFoundIndex++;
                }
            }
            // 3. On utilise l'API officielle pour réinjecter proprement
            if (bNeedsUpdate)
            {
                IMC->UnmapAll(); // On vide l'IMC

                for (const FEnhancedActionKeyMapping& Map : CopiedMappings)
                {
                    // On remap chaque touche en conservant l'ordre exact pour les SlotIndex
                    FEnhancedActionKeyMapping& NewMap = IMC->MapKey(Map.Action, Map.Key);

                    // Très important : on re-transfère les Modifiers et Triggers (ex: Pressed, Hold...)
                    NewMap.Modifiers = Map.Modifiers;
                    NewMap.Triggers = Map.Triggers;


                }
            }
        }
    }
}



// =========================
// =     GetMappingIMC	   =
// ==========================
TArray<FEnhancedActionKeyMapping> UMappingLibrary::GetMappingIMC(UInputMappingContext* IMC)
{
    if (!IMC && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Pas de IMC pour GetMappingIMC"));
        return TArray<FEnhancedActionKeyMapping>();
    }

    return IMC->GetMappings();
}





// =================================
// =     SetKeyForActionInIMC	   =
// =================================
void UMappingLibrary::SetKeyForActionInIMC(UInputMappingContext* IMC, UInputAction* Action, FKey NewKey, int32 SlotIndex, bool bWantShift, UInputAction* ModifierActionShift)
{
    if (!IMC || !Action)
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SetKeyForActionInIMC : IMC ou Action invalide"));
        return;
    }

    TArray<FEnhancedActionKeyMapping> CopiedMappings = IMC->GetMappings();
    bool bNeedsUpdate = false;
    int32 CurrentFoundIndex = 0;

    // 1. Trouver et modifier dans la copie
    for (FEnhancedActionKeyMapping& Mapping : CopiedMappings)
    {
        if (Mapping.Action && Mapping.Action == Action)
        {
            if (CurrentFoundIndex == SlotIndex)
            {
                Mapping.Key = NewKey;
                bNeedsUpdate = true;
                if (!bWantShift) {
                    Mapping.Triggers.Empty();
                }
                else {
                    // 1. Vérifier si un ChordAction pour Shift existe déjà
                    bool bHasChord = false;
                    for (UInputTrigger* Trigger : Mapping.Triggers)
                    {
                        if (UInputTriggerChordAction* Chord = Cast<UInputTriggerChordAction>(Trigger))
                        {
                            if (Chord->ChordAction == ModifierActionShift)
                            {
                                bHasChord = true;
                                break;
                            }
                        }
                    }

                    // 2. Si non, on l'ajoute
                    if (!bHasChord && ModifierActionShift)
                    {
                        // On utilise 'IMC' (l'objet valide en cours) comme parent (Outer)
                        UInputTriggerChordAction* NewChord = NewObject<UInputTriggerChordAction>(IMC, NAME_None, RF_NoFlags);
                        NewChord->ChordAction = ModifierActionShift;

                        Mapping.Triggers.Add(NewChord);
                    }
                }
                break;
            }
            CurrentFoundIndex++;
        }
    }

    // 2. Réappliquer avec l'API officielle
    if (bNeedsUpdate)
    {
        IMC->UnmapAll();

        for (const FEnhancedActionKeyMapping& Map : CopiedMappings)
        {
            FEnhancedActionKeyMapping& NewMap = IMC->MapKey(Map.Action, Map.Key);
            NewMap.Modifiers = Map.Modifiers;
            NewMap.Triggers = Map.Triggers;
        }

        // 3. Mise à jour de la sauvegarde
        UpdateSaveGame(IMC, Action, NewKey, SlotIndex, bWantShift);
    }
}


// =============================
// =     GetInputAction_Key	   =
// =============================
FInputActionKey UMappingLibrary::GetInputAction_Key(UInputMappingContext* IMC, UInputAction* Action, int32 SlotIndex)
{
    if (!IMC || !Action) return FInputActionKey();

    FString IMCName = IMC->GetName();
    FName ActionName = Action->GetFName();

    // 1. Tenter de charger depuis la sauvegarde
    UKeyBoardSave* LoadedSAVE = Cast<UKeyBoardSave>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSettings"), 0));
    if (LoadedSAVE)
    {
        for (const FInputActionKey& Data : LoadedSAVE->SaveInputAction_Key)
        {
            if (Data.SaveIMC == IMCName && Data.ActionName == ActionName && Data.SlotIndex == SlotIndex)
            {
                return Data; // Touche trouvée dans la save !
            }
        }
    }

    // 2. Si on arrive ici, la touche n'est PAS dans la save.
    // Il faut chercher la touche PAR DÉFAUT dans l'asset IMC.
    FInputActionKey DefaultData;
    DefaultData.ActionName = ActionName;
    DefaultData.SlotIndex = SlotIndex;
    DefaultData.SaveIMC = IMCName;

    // Initialiser à None par sécurité
    DefaultData.SaveKey = EKeys::Invalid;

    int32 CurrentIndex = 0;
    // On parcourt les mappings par défaut de l'asset
    for (const FEnhancedActionKeyMapping& Mapping : IMC->GetMappings())
    {
        if (Mapping.Action == Action)
        {
            if (CurrentIndex == SlotIndex)
            {
                DefaultData.SaveKey = Mapping.Key; // On a trouvé la touche par défaut !
                break;
            }
            CurrentIndex++;
        }
    }

    return DefaultData;
}


// =====================================
// =     GetActionKeyDisplayName       =
// =====================================
FText UMappingLibrary::GetActionKeyDisplayName(UInputMappingContext* IMC, UInputAction* Action, int32 SlotIndex)
{
    if (!IMC || !Action) return FText::FromString(TEXT("[Erreur]"));

    // 1. On cherche le mapping EXACT actuel dans l'IMC en mémoire
    const FEnhancedActionKeyMapping* FoundMapping = nullptr;
    int32 CurrentIndex = 0;

    for (const FEnhancedActionKeyMapping& Mapping : IMC->GetMappings())
    {
        if (Mapping.Action == Action)
        {
            if (CurrentIndex == SlotIndex)
            {
                FoundMapping = &Mapping;
                break;
            }
            CurrentIndex++;
        }
    }

    // Si on n'a rien trouvé
    if (!FoundMapping)
    {
        return FText::FromString(TEXT("Empty"));
    }

    // 2. On récupère le nom de la touche principale (ex: "A", "E")
    FText MainKeyText = GetCleanKeyDisplayName(FoundMapping->Key);

    // 3. On fouille dans les Triggers pour voir s'il y a un Chord Action (Shift, Ctrl, etc.)
    for (UInputTrigger* Trigger : FoundMapping->Triggers)
    {
        if (UInputTriggerChordAction* ChordTrigger = Cast<UInputTriggerChordAction>(Trigger))
        {
            // On a trouvé un Trigger de combinaison ! On regarde quelle action il utilise
            if (ChordTrigger->ChordAction)
            {
                UInputAction* ChordAction = const_cast<UInputAction*>(ChordTrigger->ChordAction.Get());
                // On cherche maintenant quelle touche est assignée à ce ChordAction (ex: IA_Modifier_Shift)
                for (const FEnhancedActionKeyMapping& ChordMapping : IMC->GetMappings())
                {
                    if (ChordMapping.Action == ChordAction)
                    {
                        // On récupère le nom de la touche du modificateur (ex: "Left Shift")
                        FText ChordKeyText = GetCleanKeyDisplayName(ChordMapping.Key);

                        // On fabrique et on retourne le texte combiné : "Shift + A"
                        return FText::Format(FText::FromString(TEXT("{0} + {1}")), ChordKeyText, MainKeyText);
                    }
                }
            }
        }
    }

    // Si la boucle se termine sans trouver de Chord Action, on retourne juste la touche normale
    return MainKeyText;
}


// =====================
// =     GetUseKey	   =
// =====================
TArray<FInputActionKey> UMappingLibrary::GetUseKey(FKey SearchKey, bool shift)
{
    UKeyBoardSave* LoadedSAVE = Cast<UKeyBoardSave>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSettings"), 0));
    if (!LoadedSAVE && GEngine)
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Sauvegarde vide ! pour la function GetUseKey() "));
        return TArray<FInputActionKey>();
    }
    else
    {
        TArray<FInputActionKey> AllKeyFind;
        for (const FInputActionKey& Data : LoadedSAVE->SaveInputAction_Key)
        {
            if (Data.SaveKey == SearchKey)
            {
                if (Data.bHasShiftModifier == shift)
                {
                    AllKeyFind.Add(Data);
                }
            }
        }
        return AllKeyFind;
    }
}



// =================================
// =     ClearInputAction_Key	   =
// =================================
void UMappingLibrary::ClearInputAction_Key()
{
    // Charger la sauvegarde existante
    UKeyBoardSave* LoadedSAVE = Cast<UKeyBoardSave>(
        UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSettings"), 0)
    );

    if (!LoadedSAVE)
    {
        LoadedSAVE = Cast<UKeyBoardSave>(
            UGameplayStatics::CreateSaveGameObject(UKeyBoardSave::StaticClass())
        );
    }

    // Vider le contenu du tableau
    LoadedSAVE->SaveInputAction_Key.Empty();

    // Sauvegarder la modification
    UGameplayStatics::SaveGameToSlot(LoadedSAVE, TEXT("PlayerSettings"), 0);
}


// =====================
// =     DebugLog	   =
// =====================
void UMappingLibrary::DebugLogMapping()
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan,
            FString::Printf(TEXT("OS Keyboard: %s"), *WhatTypeKeyboard()));

        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan,
            FString::Printf(TEXT("Saved Layout: %s"), *LoadKeyboardSave()));
    }

    UKeyBoardSave* LoadedSAVE = Cast<UKeyBoardSave>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSettings"), 0));

    if (LoadedSAVE)
    {
        if (LoadedSAVE->SaveInputAction_Key.Num() == 0)
        {
            if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Sauvegarde vide !"));
        }
        else
        {
            for (const FInputActionKey& Data : LoadedSAVE->SaveInputAction_Key)
            {
                // On prépare le message
                FString Msg = FString::Printf(TEXT("IMC: %s | Action: %s | Index: %d | Key: %s"),
                    *Data.SaveIMC,
                    *Data.ActionName.ToString(), // Utilise ActionName au lieu de SaveInputAction
                    Data.SlotIndex,             // Affiche l'index (0, 1, etc.)
                    *Data.SaveKey.GetDisplayName().ToString()
                );

                // Log à l'écran
                if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, Msg);

                // Log dans la console (Output Log)
                UE_LOG(LogTemp, Log, TEXT("%s"), *Msg);
            }
        }
    }
    else
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("Aucun fichier de sauvegarde PlayerSettings trouve."));
    }
}



// =============================
// =     LoadKeyboardSave	   =
// =============================
FString UMappingLibrary::LoadKeyboardSave()
{
    if (UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSettings"), 0))
    {
        UKeyBoardSave* LoadedGame = Cast<UKeyBoardSave>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSettings"), 0));
        if (LoadedGame)
        {
            return LoadedGame->KeyboardLayout;
        }
    }

    // Si aucun fichier trouvé alors valeur par défaut
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Pas de sauvegarde faite on retourne par défault en FR"));
    }
    return "fr-FR";
}



// =============================
// =     GetSimpleFloatSave	   =
// =============================
float UMappingLibrary::GetSimpleFloatSave(FString NameOptionSave)
{

    if (!UGameplayStatics::DoesSaveGameExist("SettingGlobal", 0))
    {
        return 1.0f;
    }

    // Elle existe donc on la charge
    UKeyBoardSave* LoadedGame = Cast<UKeyBoardSave>(
        UGameplayStatics::LoadGameFromSlot("SettingGlobal", 0)
    );

    if (!LoadedGame)
    {
        FString DebugMessage = FString::Printf(TEXT("ERREUR: Sauvegarde SettingGlobal introuvable alors que normalement si."));
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, DebugMessage);
        return 1.0f;
    }

    float* FoundFloat = LoadedGame->SaveSimpleFloat.Find(NameOptionSave);

    if (FoundFloat != nullptr)
    {
        // retourner la valeur sauvegarder
        return *FoundFloat;
    }
    else
    {
        //if (GEngine)
        //{
        //    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan,
        //        FString::Printf(TEXT("Echec de l'extraction du float pour %s dans GetSimpleFloatSave"), *NameOptionSave));
        //}

        return 1.0f;
    }


    
}




// =============================
// =    SaveSimpleFloatSave    =
// =============================
void UMappingLibrary::SaveSimpleFloatSave(FString NameOptionSave, float FloatToSave)
{
    UKeyBoardSave* SaveGameInstance = nullptr;

    // 1. On essaie d'abord de charger la sauvegarde si elle existe
    if (UGameplayStatics::DoesSaveGameExist("SettingGlobal", 0))
    {
        SaveGameInstance = Cast<UKeyBoardSave>(UGameplayStatics::LoadGameFromSlot("SettingGlobal", 0));
    }

    // 2. Si elle n'existait pas (ou que le chargement a échoué), on la crée
    if (!SaveGameInstance)
    {
        SaveGameInstance = Cast<UKeyBoardSave>(UGameplayStatics::CreateSaveGameObject(UKeyBoardSave::StaticClass()));
    }

    // Garde-fou ultime au cas où il y aurait un bug critique du moteur
    if (!SaveGameInstance)
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("ERREUR CRITIQUE: Impossible de créer ou charger SettingGlobal"));
        return;
    }

    // 3. Modifier les données
    SaveGameInstance->SaveSimpleFloat.Emplace(NameOptionSave, FloatToSave);

    // 4. Sauvegarder sur le disque
    bool bSuccess = UGameplayStatics::SaveGameToSlot(SaveGameInstance, "SettingGlobal", 0);

    if (!bSuccess && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("ÉCHEC: Impossible de sauvegarder dans le slot SettingGlobal."));
    }
}




// =====================
// =    GetFontSave    =
// =====================
UFont* UMappingLibrary::GetFontSave()
{

    if (!UGameplayStatics::DoesSaveGameExist("SettingGlobal", 0))
    {
        return nullptr;
    }

    UKeyBoardSave* LoadedGame = Cast<UKeyBoardSave>(UGameplayStatics::LoadGameFromSlot("SettingGlobal", 0));

    if (LoadedGame && !LoadedGame->FontSave.IsNull())
    {
        // On charge l'asset en mémoire de manière synchrone (LoadSynchronous)
        // Cela donne un UFont* classique
        UFont* FontUtilisable = LoadedGame->FontSave.LoadSynchronous();

        return FontUtilisable;
    }

    return nullptr;
}





// ======================
// =    SaveFontSave    =
// ======================
void UMappingLibrary::SaveFontSave(UFont* FontToSave)
{
    UKeyBoardSave* SaveGameInstance = nullptr;

    // 1. On essaie d'abord de charger la sauvegarde si elle existe
    if (UGameplayStatics::DoesSaveGameExist("SettingGlobal", 0))
    {
        SaveGameInstance = Cast<UKeyBoardSave>(UGameplayStatics::LoadGameFromSlot("SettingGlobal", 0));
    }

    // 2. Si elle n'existait pas (ou que le chargement a échoué), on la crée
    if (!SaveGameInstance)
    {
        SaveGameInstance = Cast<UKeyBoardSave>(UGameplayStatics::CreateSaveGameObject(UKeyBoardSave::StaticClass()));
    }

    // Garde-fou ultime au cas où il y aurait un bug critique du moteur
    if (!SaveGameInstance)
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("ERREUR CRITIQUE: Impossible de créer ou charger SettingGlobal"));
        return;
    }

    if (FontToSave)
    {
        // On extrait le chemin propre de l'asset et on l'assigne à notre variable de sauvegarde
        SaveGameInstance->FontSave = FontToSave;
    }

    // 4. Sauvegarder sur le disque
    bool bSuccess = UGameplayStatics::SaveGameToSlot(SaveGameInstance, "SettingGlobal", 0);

    if (!bSuccess && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("ÉCHEC: Impossible de sauvegarder dans le slot SettingGlobal."));
    }
}







// =========================
// =     UpdateSaveGame	   =
// =========================
void UMappingLibrary::UpdateSaveGame(UInputMappingContext* IMC, UInputAction* Action, FKey NewKey, int32 SlotIndex, bool bWantShift)
{
    UKeyBoardSave* LoadedSAVE = Cast<UKeyBoardSave>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSettings"), 0));
    if (!LoadedSAVE)
    {
        LoadedSAVE = Cast<UKeyBoardSave>(UGameplayStatics::CreateSaveGameObject(UKeyBoardSave::StaticClass()));

        LoadedSAVE->KeyboardLayout = TEXT("fr-FR");
    }

    FString IMCName = IMC->GetName();
    FName ActionName = Action->GetFName();
    bool bFound = false;
    for (FInputActionKey& Data : LoadedSAVE->SaveInputAction_Key)
    {
        // On cherche par Nom d'Action et Index de touche
        if (Data.ActionName == ActionName && Data.SlotIndex == SlotIndex && Data.SaveIMC == IMCName)
        {
            Data.SaveKey = NewKey;
            Data.bHasShiftModifier = bWantShift;
            bFound = true;
            break;
        }
    }

    // Si on n'a pas trouvé d'entrée existante, on en crée une nouvelle
    if (!bFound)
    {
        FInputActionKey NewEntry;
        NewEntry.SaveIMC = IMCName;
        NewEntry.ActionName = ActionName;
        NewEntry.SaveKey = NewKey;
        NewEntry.SlotIndex = SlotIndex;
        NewEntry.bHasShiftModifier = bWantShift;
        LoadedSAVE->SaveInputAction_Key.Add(NewEntry);
    }

    UGameplayStatics::SaveGameToSlot(LoadedSAVE, TEXT("PlayerSettings"), 0);
}


// =====================================
// =     GetCleanKeyDisplayName        =
// =====================================
FText UMappingLibrary::GetCleanKeyDisplayName(FKey Key)
{
    if (Key.IsValid())
    {
        FString KeyName = Key.ToString();

        //if (GEngine)
        //{
        //    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("KeyName raw: %s"), *KeyName));
        //}

        // --- CORRECTION SPÉCIALE CLAVIER AZERTY ---
        if (KeyName.Equals(TEXT("Ampersand"), ESearchCase::IgnoreCase))       return FText::FromString(TEXT("1"));
        if (KeyName.Equals(TEXT("E_AccentAigu"), ESearchCase::IgnoreCase))    return FText::FromString(TEXT("2"));
        if (KeyName.Equals(TEXT("Quote"), ESearchCase::IgnoreCase))           return FText::FromString(TEXT("3"));
        if (KeyName.Equals(TEXT("Apostrophe"), ESearchCase::IgnoreCase))      return FText::FromString(TEXT("4"));
        if (KeyName.Equals(TEXT("LeftParenthesis"), ESearchCase::IgnoreCase)) return FText::FromString(TEXT("5"));
        if (KeyName.Equals(TEXT("Minus"), ESearchCase::IgnoreCase) || KeyName.Equals(TEXT("Hyphen"), ESearchCase::IgnoreCase))
        {
            return FText::FromString(TEXT("6"));
        }
        if (KeyName.Equals(TEXT("E_AccentGrave"), ESearchCase::IgnoreCase))   return FText::FromString(TEXT("7"));
        if (KeyName.Equals(TEXT("Underscore"), ESearchCase::IgnoreCase))      return FText::FromString(TEXT("8"));
        if (KeyName.Equals(TEXT("C_Cedille"), ESearchCase::IgnoreCase))       return FText::FromString(TEXT("9"));
        if (KeyName.Equals(TEXT("A_AccentGrave"), ESearchCase::IgnoreCase))   return FText::FromString(TEXT("0"));
        if (KeyName.Equals(TEXT("LeftShift"), ESearchCase::IgnoreCase))   return FText::FromString(TEXT("Shift"));
        if (KeyName.Equals(TEXT("RightShift"), ESearchCase::IgnoreCase))   return FText::FromString(TEXT("Shift"));

        // --- COMPORTEMENT PAR DÉFAUT ---
        return Key.GetDisplayName();
    }

    return FText::FromString(TEXT("[Touche Non Assignée]"));
}

