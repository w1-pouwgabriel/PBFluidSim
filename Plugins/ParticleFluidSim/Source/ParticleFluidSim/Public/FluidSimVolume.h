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

    UFUNCTION(BlueprintCallable, Category="FluidSim")
    void SetVolumeLocation(FVector NewLocation)
    {
        SetActorLocation(NewLocation);
    }

    void HandleParticleCollisions();

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
