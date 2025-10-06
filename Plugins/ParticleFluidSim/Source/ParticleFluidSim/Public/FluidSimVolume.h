// FluidSimVolume.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FluidParcelActor.h"
#include "FluidSimVolume.generated.h"

UCLASS()
class PARTICLEFLUIDSIM_API AFluidSimVolume : public AActor
{
    GENERATED_BODY()

public:
    AFluidSimVolume();

private:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // SPH constants
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SPH")
    float RestDensity = 1000.f; // kg/m^3

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SPH")
    float GasConstant = 2000.f; // stiffness constant

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SPH")
    float Viscosity = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SPH")
    float SmoothingRadius = 50.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SPH")
    float Gravity = -980.f; // Unreal units (cm/s²)

    // Size of the simulation volume (half extents of the box)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fluid Sim")
    FVector VolumeHalfExtents;

    // Number of parcels to spawn
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fluid Sim")
    int32 NumParcels;

    // Visual debug of the bounding box
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fluid Sim")
    bool bDrawDebugBox;

    // List of spawned parcels
    UPROPERTY()
    TArray<TObjectPtr<AFluidParcelActor>> Parcels;
};
