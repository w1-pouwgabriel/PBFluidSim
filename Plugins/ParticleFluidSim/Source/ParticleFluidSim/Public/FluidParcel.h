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
    float Mass = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fluid Parcel")
    float Density = 1000.0f;

    /** Pressure from equation of state */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Fluid Parcel")
    float Pressure;

    /** Accumulated force (from SPH pressure, viscosity, gravity, etc.) */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Fluid Parcel")
    FVector Force;

    void ResetForces(float DeltaTime);
};
