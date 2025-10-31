// FluidSimVolume.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FluidParcelActor.h"
#include "FluidSimVolume.generated.h"

UENUM(BlueprintType)
enum class EBorderType : uint8
{
    Soft UMETA(DisplayName = "Soft"),
    Hard UMETA(DisplayName = "Hard")
};

UCLASS()
class PARTICLEFLUIDSIM_API AFluidSimVolume : public AActor
{
    GENERATED_BODY()

public:
    AFluidSimVolume();

protected:
    virtual void BeginPlay() override;

    void SoftCollisionCheck(float DeltaTime);
    void HardCollisionCheck(float DeltaTime);

    // Add particles on a specific (spot or have a spawner attached to a separate AActor)
    void AddParticles(uint32 NumParticles);

public:
    virtual void Tick(float DeltaTime) override;

    // Border vals
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fluid Sim")
    EBorderType BorderType = EBorderType::Soft;

    // SPH constants
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SPH")
    float RestDensity = 1.0f; // kg/m^3

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SPH")
    float GasConstant = 2000.0f; // stiffness

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SPH")
    float Viscosity = 5.0f;

    UPROPERTY(VisibleAnywhere, Category = "SPH")
    float SmoothingRadius = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SPH")
<<<<<<< HEAD
    float Gravity = -980.f; // Unreal units (cm/s^2)
=======
    float Gravity = -980.f; // Unreal units (cm/s�)
>>>>>>> a5a8767bb20addb08067ca0fbc80a78fc5684b0b

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
