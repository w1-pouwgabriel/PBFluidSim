// Copyright Epic Games, Inc. All Rights Reserved.

#include "ParticleFluidSim.h"
#include "GameFramework/Actor.h"
#include "FluidSimPlane.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "FParticleFluidSimModule"

void FParticleFluidSimModule::StartupModule()
{
    FWorldDelegates::OnPostWorldInitialization.AddLambda([](UWorld* World, const UWorld::InitializationValues IVS)
    {
        if (World && World->IsGameWorld())
        {
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			FVector SpawnLocation(-500.f, 0.f, 100.f); //(X=-500.000000,Y=0.000000,Z=100.000000)
			FRotator Rot(-90.f, 0, 0.f);
			FVector Scale(1.f, 1.f, 1.f);

			FTransform SpawnTransform(Rot, SpawnLocation, Scale);

			// now spawn using transform
			AFluidSimPlane* Plane = World->SpawnActor<AFluidSimPlane>(
				AFluidSimPlane::StaticClass(),
				SpawnTransform,
				Params
			);
        }
    });
}

void FParticleFluidSimModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FParticleFluidSimModule, ParticleFluidSim)