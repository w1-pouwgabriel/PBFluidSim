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

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    void SetLinearColor(float VaLue);

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> SphereMesh;

    UPROPERTY()
    TObjectPtr<UFluidParcel> Parcel;

    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> DynamicMat;
};
