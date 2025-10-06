// FluidParcelActor.cpp
#include "FluidParcelActor.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AFluidParcelActor::AFluidParcelActor()
{
    PrimaryActorTick.bCanEverTick = true;

    SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));
    RootComponent = SphereMesh;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshObj(TEXT("/Engine/BasicShapes/Sphere"));
    if (SphereMeshObj.Succeeded())
    {
        SphereMesh->SetStaticMesh(SphereMeshObj.Object);
        SphereMesh->SetWorldScale3D(FVector(0.1f)); // smaller spheres
    }

    Parcel = NewObject<UFluidParcel>();
}

void AFluidParcelActor::InitParcel(FVector StartPos, FVector StartVel)
{
    Parcel->Position = StartPos;
    Parcel->Velocity = StartVel;
    SetActorLocation(StartPos);
}

void AFluidParcelActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (Parcel)
    {
        SetActorLocation(Parcel->Position); // move the sphere
    }
}
