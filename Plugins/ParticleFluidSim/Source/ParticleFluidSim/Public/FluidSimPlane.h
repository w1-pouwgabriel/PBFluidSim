#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FluidSimPlane.generated.h"

UCLASS()
class PARTICLEFLUIDSIM_API AFluidSimPlane : public AActor
{
    GENERATED_BODY()

public:
    AFluidSimPlane();

    /** Updates the texture shown on the plane */
    void UpdateFluidTexture(UTexture2D* NewTexture);

protected:
    UPROPERTY(VisibleAnywhere, Category="Components")
    class UStaticMeshComponent* PlaneMesh;

    UPROPERTY(VisibleAnywhere, Category="Materials")
    class UMaterialInstanceDynamic* DynamicMaterial;
};
