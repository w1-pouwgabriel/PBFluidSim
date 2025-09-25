#include "FluidParcel.h"

UFluidParcel::UFluidParcel()
{
    Position = FVector::ZeroVector;
    Velocity = FVector::ZeroVector;
    Mass = 2.0f;
    Density = 20.0f;
}

void UFluidParcel::Update(float DeltaTime)
{
    // Gravity force
    FVector Force = FVector(0.f, 0.f, -980.f) * Mass;
    // Acceleration = Force / Mass  → cancels mass out
    FVector Acceleration = Force / Mass; 
    // Update velocity and position
    Velocity += Acceleration * DeltaTime;
    Position += Velocity * DeltaTime;
}
