// FluidParcelActor.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FluidParcel.h"

#include "FluidParcelActor.generated.h"

UCLASS()
class PARTICLEFLUIDSIM_API AFluidParcelActor : public AActor
{
    GENERATED_BODY()

public:
    AFluidParcelActor();

    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> SphereMesh;

    UPROPERTY()
    TObjectPtr<UFluidParcel> Parcel;

    void InitParcel(FVector StartPos, FVector StartVel);
};
