#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FluidParcel.generated.h"


UCLASS()
class PARTICLEFLUIDSIM_API UFluidParcel : public UObject
{
    GENERATED_BODY()

public:
    UFluidParcel() {};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fluid Parcel")
    FVector Position = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fluid Parcel")
    FVector Velocity = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fluid Parcel")
    float Mass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fluid Parcel")
    float Density;

    /** Pressure from equation of state */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Fluid Parcel")
    float Pressure;

    /** Accumulated force (from SPH pressure, viscosity, gravity, etc.) */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Fluid Parcel")
    FVector Force;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Fluid Parcel")
    bool bIsBoundary = false; // True if this is a ghost/boundary particle
};
