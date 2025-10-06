#include "FluidParcelActor.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"

AFluidParcelActor::AFluidParcelActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    // Create mesh
    SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));
    RootComponent = SphereMesh;

    // Load engine sphere mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshObj(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (SphereMeshObj.Succeeded())
    {
        SphereMesh->SetStaticMesh(SphereMeshObj.Object);
        SphereMesh->SetWorldScale3D(FVector(0.1f)); // smaller spheres
    }

    // Load base material
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialObj(TEXT("/Game/Materials/M_FluidParcel.M_FluidParcel"));
    if (MaterialObj.Succeeded())
    {
        SphereMesh->SetMaterial(0, MaterialObj.Object);
    }

    // Create parcel
    Parcel = NewObject<UFluidParcel>();
}

void AFluidParcelActor::BeginPlay()
{
    Super::BeginPlay();

    // Create dynamic material instance
    if (SphereMesh && SphereMesh->GetMaterial(0))
    {
        DynamicMat = UMaterialInstanceDynamic::Create(SphereMesh->GetMaterial(0), this);
        SphereMesh->SetMaterial(0, DynamicMat);
    }

    UE_LOG(LogTemp, Warning, TEXT("AFluidParcelActor BeginPlay: %s"), *GetName());
}

void AFluidParcelActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (Parcel && DynamicMat)
    {
        float Speed = Parcel->Velocity.Size();
        float MaxExpectedSpeed = 500.f; // adjust to your simulation

        float NormalizedSpeed = FMath::Clamp(Speed / MaxExpectedSpeed, 0.f, 1.f);
        NormalizedSpeed = FMath::Pow(NormalizedSpeed, 0.5f); // exaggerate slow speeds

        FLinearColor SpeedColor = FLinearColor::LerpUsingHSV(FLinearColor::Blue, FLinearColor::Red, NormalizedSpeed);

        DynamicMat->SetVectorParameterValue(TEXT("PressureColor"), SpeedColor);
    }

    Parcel->ResetForces(DeltaTime);
}

