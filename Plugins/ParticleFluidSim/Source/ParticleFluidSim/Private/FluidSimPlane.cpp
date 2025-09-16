#include "FluidSimPlane.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"

AFluidSimPlane::AFluidSimPlane()
{
    PrimaryActorTick.bCanEverTick = false;

    PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh"));
    RootComponent = PlaneMesh;

    // Use the built-in Engine Plane mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMeshObj(TEXT("/Engine/BasicShapes/Plane"));
    if (PlaneMeshObj.Succeeded())
    {
        PlaneMesh->SetStaticMesh(PlaneMeshObj.Object);
    }

    // Apply a default material
    static ConstructorHelpers::FObjectFinder<UMaterial> MaterialObj(TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
    if (MaterialObj.Succeeded())
    {
        DynamicMaterial = UMaterialInstanceDynamic::Create(MaterialObj.Object, this);
        PlaneMesh->SetMaterial(0, DynamicMaterial);
    }
}

void AFluidSimPlane::UpdateFluidTexture(UTexture2D* NewTexture)
{
    if (DynamicMaterial && NewTexture) // Check if 
    {
        DynamicMaterial->SetTextureParameterValue(FName("Texture"), NewTexture);
    }
}