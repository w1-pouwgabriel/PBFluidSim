#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FluidParcel.generated.h"


UCLASS()
class PARTICLEFLUIDSIM_API UFluidParcel : public UObject
{
    GENERATED_BODY()

public:
    UFluidParcel();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fluid Parcel")
    FVector Position;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fluid Parcel")
    FVector Velocity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fluid Parcel")
    float Mass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fluid Parcel")
    float Density;
};
