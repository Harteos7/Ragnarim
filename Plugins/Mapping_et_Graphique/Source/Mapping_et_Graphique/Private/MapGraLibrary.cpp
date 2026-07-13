// Copyright Epic Games, Inc. All Rights Reserved.

#include "MapGraLibrary.h"

#define LOCTEXT_NAMESPACE "FMapping_et_GraphiqueModule"

void FMapping_et_GraphiqueModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FMapping_et_GraphiqueModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMapping_et_GraphiqueModule, Mapping_et_Graphique)