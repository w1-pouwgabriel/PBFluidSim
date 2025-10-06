#include "FluidParcel.h"

void UFluidParcel::ResetForces(float DeltaTime)
{
    FVector Acceleration = Force / Density;
    Velocity += Acceleration * DeltaTime;
    Position += Velocity * DeltaTime;
    Force = FVector::ZeroVector; // reset for next frame
}
