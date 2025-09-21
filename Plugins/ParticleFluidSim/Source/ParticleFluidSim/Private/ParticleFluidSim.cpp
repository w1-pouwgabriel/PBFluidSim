// Copyright Epic Games, Inc. All Rights Reserved.

#include "ParticleFluidSim.h"
#include "GameFramework/Actor.h"
#include "FluidSimVolume.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "FParticleFluidSimModule"

void FParticleFluidSimModule::StartupModule()
{
    
}

void FParticleFluidSimModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FParticleFluidSimModule, ParticleFluidSim)