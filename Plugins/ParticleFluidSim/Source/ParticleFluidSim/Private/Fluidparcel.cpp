#include "FluidParcel.h"

UFluidParcel::UFluidParcel()
{
    Position = FVector::ZeroVector;
    Velocity = FVector::ZeroVector;
    Mass = 1.0f;
    Density = 2.0f;
}

void UFluidParcel::Update(float DeltaTime)
{
    const FVector Gravity(0.f, 0.f, -980.f);

    Velocity += Gravity * DeltaTime;
    Position += Velocity * DeltaTime;
}
