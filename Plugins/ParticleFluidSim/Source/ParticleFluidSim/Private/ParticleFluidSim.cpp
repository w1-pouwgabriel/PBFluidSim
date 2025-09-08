// Copyright Epic Games, Inc. All Rights Reserved.

#include "ParticleFluidSim.h"

DEFINE_LOG_CATEGORY(LogParticleFluidSim);

#define LOCTEXT_NAMESPACE "FParticleFluidSimModule"

void FParticleFluidSimModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	UE_LOG(LogParticleFluidSim, Log, TEXT("Hello World"));
}

void FParticleFluidSimModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FParticleFluidSimModule, ParticleFluidSim)