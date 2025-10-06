#include "FluidParcel.h"

UFluidParcel::UFluidParcel()
{
    Position = FVector::ZeroVector;
    Velocity = FVector::ZeroVector;
    Mass = 2.0f;
    Density = 20.0f;
}