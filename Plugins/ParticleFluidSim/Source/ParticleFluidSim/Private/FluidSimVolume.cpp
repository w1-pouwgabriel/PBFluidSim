// FluidSimVolume.cpp
#include "FluidSimVolume.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Helper/SPHMath.h"

AFluidSimVolume::AFluidSimVolume()
    : VolumeHalfExtents(FVector(250.f, 250.f, 250.f)), NumParcels(500), bDrawDebugBox(true)
{
    PrimaryActorTick.bCanEverTick = true;
}

void AFluidSimVolume::BeginPlay()
{
    Super::BeginPlay();

    FVector VolumeOrigin = GetActorLocation();
    float TopZ = VolumeOrigin.Z + VolumeHalfExtents.Z - 5.f; // slightly below top
    int32 NumPerRow = FMath::CeilToInt(FMath::Sqrt((float)NumParcels));
    float SpacingX = (VolumeHalfExtents.X * 2.f) / NumPerRow;
    float SpacingY = (VolumeHalfExtents.Y * 2.f) / NumPerRow;

    int32 ParticleCount = 0;

    for (int32 ix = 0; ix < NumPerRow; ix++)
    {
        for (int32 iy = 0; iy < NumPerRow; iy++)
        {
            if (ParticleCount >= NumParcels)
                break;

            FVector RandPos;
            RandPos.X = VolumeOrigin.X - VolumeHalfExtents.X + SpacingX * ix + SpacingX * 0.5f;
            RandPos.Y = VolumeOrigin.Y - VolumeHalfExtents.Y + SpacingY * iy + SpacingY * 0.5f;
            RandPos.Z = TopZ;

            // Optionally add a tiny random offset to avoid perfect grid alignment
            float Jitter = 1.0f; // 1 cm jitter
            RandPos.X += FMath::FRandRange(-Jitter, Jitter);
            RandPos.Y += FMath::FRandRange(-Jitter, Jitter);

            FVector RandVel = FVector::ZeroVector; // start still, or small random velocity if desired

            AFluidParcelActor* ParcelActor = GetWorld()->SpawnActor<AFluidParcelActor>(RandPos, FRotator::ZeroRotator);
            if (ParcelActor)
            {
                ParcelActor->Parcel->Position = RandPos;
                ParcelActor->Parcel->Velocity = RandVel;
                Parcels.Add(ParcelActor);
            }

            ParticleCount++;
        }
    }

    FVector VolumeSize = VolumeHalfExtents * 2.f; // full size
    float Volume = VolumeSize.X * VolumeSize.Y * VolumeSize.Z;

    float ParticleSpacing = FMath::Pow(Volume / NumParcels, 1.f / 3.f);

    // Set smoothing radius to 1.5x spacing
    SmoothingRadius = ParticleSpacing * 1.5f;

    UE_LOG(LogTemp, Log, TEXT("Auto SmoothingRadius: %f (Particle spacing: %f)"), SmoothingRadius, ParticleSpacing);
}

void AFluidSimVolume::VolumeCollisionCheck()
{
    const float BounceDamping = 1.f;

    for (AFluidParcelActor* Parcel : Parcels)
    {
        if (!Parcel || !Parcel->Parcel) continue;

        FVector& Pos = Parcel->Parcel->Position;
        FVector& Vel = Parcel->Parcel->Velocity;

        FVector MinBounds = GetActorLocation() - VolumeHalfExtents;
        FVector MaxBounds = GetActorLocation() + VolumeHalfExtents;

        // X axis
        if (Pos.X < MinBounds.X) { Pos.X = MinBounds.X; Vel.X = Vel.X * -1.f; }
        else if (Pos.X > MaxBounds.X) { Pos.X = MaxBounds.X; Vel.X = Vel.X * -1.f; }

        // Y axis
        if (Pos.Y < MinBounds.Y) { Pos.Y = MinBounds.Y; Vel.Y = Vel.Y * -1.f; }
        else if (Pos.Y > MaxBounds.Y) { Pos.Y = MaxBounds.Y; Vel.Y = Vel.Y * -1.f; }

        // Z axis
        if (Pos.Z < MinBounds.Z) { Pos.Z = MinBounds.Z; Vel.Z = 0.f; }
        else if (Pos.Z > MaxBounds.Z) { Pos.Z = MaxBounds.Z; Vel.Z = 0.f; }

        Parcel->SetActorLocation(Pos);
    }
}

void AFluidSimVolume::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Draw debug bounding box
    if (bDrawDebugBox)
    {
        DrawDebugBox(GetWorld(), GetActorLocation(), VolumeHalfExtents, FColor::Green, false, -1.f, 0, 2.f);
    }

    // Compute dynamic smoothing radius
    FVector VolumeSize = VolumeHalfExtents * 2.f;
    float VolumeTotal = VolumeSize.X * VolumeSize.Y * VolumeSize.Z;
    float ParticleSpacing = FMath::Pow(VolumeTotal / FMath::Max(NumParcels, 1), 1.f / 3.f);
    float h = ParticleSpacing * 1.1f; // smoothing radius

    // Compute density and pressure
    for (AFluidParcelActor* PiActor : Parcels)
    {
        UFluidParcel* Pi = PiActor->Parcel;
        if (!Pi) continue;

        Pi->Density = 0.f;

        for (AFluidParcelActor* PjActor : Parcels)
        {
            UFluidParcel* Pj = PjActor->Parcel;
            if (!Pj) continue;

            FVector Rij = Pi->Position - Pj->Position;
            float r = Rij.Size();

            Pi->Density += Pj->Mass * SPH::Poly6(r, h);
        }

        const float DensityEps = 1e-3f;
        Pi->Density = FMath::Max(Pi->Density, DensityEps);

        Pi->Pressure = GasConstant * (Pi->Density - RestDensity);
        Pi->Pressure = FMath::Clamp(Pi->Pressure, -2000.f, 20000.f);
    }

    // Compute forces and integrate
    const float MaxForce = 5000.f; // clamp force for stability

    for (AFluidParcelActor* PiActor : Parcels)
    {
        UFluidParcel* Pi = PiActor->Parcel;
        if (!Pi) continue;

        FVector PressureForce = FVector::ZeroVector;
        FVector ViscosityForce = FVector::ZeroVector;

        for (AFluidParcelActor* PjActor : Parcels)
        {
            if (PiActor == PjActor) continue;
            UFluidParcel* Pj = PjActor->Parcel;
            if (!Pj) continue;

            FVector Rij = Pi->Position - Pj->Position;
            float r = Rij.Size();

            if (r < h && r > 0.f)
            {
                FVector gradW = SPH::SpikyGradient(Rij, h);
                PressureForce += Pj->Mass * (Pi->Pressure + Pj->Pressure) / (2.f * Pj->Density) * gradW;

                float lap = SPH::ViscosityLaplacian(r, h);
                ViscosityForce += Viscosity * Pj->Mass * (Pj->Velocity - Pi->Velocity) / Pj->Density * lap;
            }
        }

        // Gravity based on mass, not density
        FVector GravityForce = FVector(0.f, 0.f, Gravity) * Pi->Mass;

        // Clamp forces to prevent instability
        PressureForce = PressureForce.GetClampedToMaxSize(MaxForce);
        ViscosityForce = ViscosityForce.GetClampedToMaxSize(MaxForce);

        FVector Acceleration = (PressureForce + ViscosityForce + GravityForce) / Pi->Mass;

        // Semi-implicit Euler integration
        Pi->Velocity += Acceleration * DeltaTime;
        Pi->Position += Pi->Velocity * DeltaTime;
    }

    VolumeCollisionCheck();
}