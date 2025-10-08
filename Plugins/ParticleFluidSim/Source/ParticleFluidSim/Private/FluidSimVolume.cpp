// FluidSimVolume.cpp
#include "FluidSimVolume.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Helper/SPHMath.h"

AFluidSimVolume::AFluidSimVolume()
    : VolumeHalfExtents(FVector(250.f, 250.f, 250.f)), NumParcels(25), bDrawDebugBox(true)
{
    PrimaryActorTick.bCanEverTick = true;
}

void AFluidSimVolume::BeginPlay()
{
    Super::BeginPlay();

    FVector VolumeOrigin = GetActorLocation();
    FVector VolumeSize = VolumeHalfExtents * 2.f * GetActorScale();
    float VolumeTotal = VolumeSize.X * VolumeSize.Y * VolumeSize.Z;

    // Estimate cube root grid dimension
    int32 NumPerAxis = FMath::RoundToInt(FMath::Pow((float)NumParcels, 1.f / 3.f));
    NumPerAxis = FMath::Max(1, NumPerAxis); // safety

    // Compute actual number of particles (some rounding may occur)
    int32 ActualNumParcels = NumPerAxis * NumPerAxis * NumPerAxis;

    // Compute particle spacing
    float ParticleSpacing = FMath::Min3(
        VolumeSize.X / NumPerAxis,
        VolumeSize.Y / NumPerAxis,
        VolumeSize.Z / NumPerAxis
    );

    SmoothingRadius = ParticleSpacing * 1.5f;

    // Start at minimum corner
    FVector StartPos = VolumeOrigin - VolumeHalfExtents + FVector(ParticleSpacing * 0.5f);
    int32 ParticleCount = 0;

    for (int32 ix = 0; ix < NumPerAxis; ix++)
    {
        for (int32 iy = 0; iy < NumPerAxis; iy++)
        {
            for (int32 iz = 0; iz < NumPerAxis; iz++)
            {
                if (ParticleCount >= NumParcels)
                    break;

                FVector Pos;
                Pos.X = StartPos.X + ix * ParticleSpacing;
                Pos.Y = StartPos.Y + iy * ParticleSpacing;
                Pos.Z = StartPos.Z + iz * ParticleSpacing;

                FVector Vel = FVector::ZeroVector;

                AFluidParcelActor* ParcelActor = GetWorld()->SpawnActor<AFluidParcelActor>(Pos, FRotator::ZeroRotator);
                if (ParcelActor)
                {
                    ParcelActor->Parcel->Position = Pos;
                    ParcelActor->Parcel->Velocity = Vel;
                    Parcels.Add(ParcelActor);
                }

                ParticleCount++;
            }
        }
    }
}


void AFluidSimVolume::VolumeCollisionCheck(float DeltaTime)
{
    const float BoundaryPressureStrength = 0.1f * RestDensity;
    const float Damping = 0.4f;

    FVector MinBounds = GetActorLocation() - VolumeHalfExtents * GetActorScale();
    FVector MaxBounds = GetActorLocation() + VolumeHalfExtents * GetActorScale();

    for (auto Parcel : Parcels) 
    {

        FVector& Pos = Parcel->Parcel->Position;
        FVector& Vel = Parcel->Parcel->Velocity;

        // Distance to walls
        FVector DistToMin = Pos - MinBounds;
        FVector DistToMax = MaxBounds - Pos;

        FVector WallForce = FVector::ZeroVector;

        // X
        if (DistToMin.X < 0.f) // below min X wall
        {
            float Penetration = -DistToMin.X;
            WallForce.X += BoundaryPressureStrength * Penetration;
        }
        else if (DistToMax.X < 0.f) // beyond max X wall
        {
            float Penetration = -DistToMax.X;
            WallForce.X -= BoundaryPressureStrength * Penetration;
        }

        // Y
        if (DistToMin.Y < 0.f)
        {
            float Penetration = -DistToMin.Y;
            WallForce.Y += BoundaryPressureStrength * Penetration;
        }
        else if (DistToMax.Y < 0.f)
        {
            float Penetration = -DistToMax.Y;
            WallForce.Y -= BoundaryPressureStrength * Penetration;
        }

        // Z
        if (DistToMin.Z < 0.f)
        {
            float Penetration = -DistToMin.Z;
            WallForce.Z += BoundaryPressureStrength * Penetration;
        }
        else if (DistToMax.Z < 0.f)
        {
            float Penetration = -DistToMax.Z;
            WallForce.Z -= BoundaryPressureStrength * Penetration;
        }
        
        WallForce = WallForce.GetClampedToMaxSize(1000.f);

        // Apply the wall pressure force (like neighboring particle pressure)
        Vel += (WallForce / Parcel->Parcel->Mass) * DeltaTime;

        // Add damping when very close to the wall to avoid jitter
        if ((DistToMin.X < 1.f) || (DistToMax.X < 1.f) ||
            (DistToMin.Y < 1.f) || (DistToMax.Y < 1.f) ||
            (DistToMin.Z < 1.f) || (DistToMax.Z < 1.f))
        {
            Vel *= (1.f - Damping * DeltaTime);
        }

        // Clamp particle inside the volume
        Pos.X = FMath::Clamp(Pos.X, MinBounds.X, MaxBounds.X);
        Pos.Y = FMath::Clamp(Pos.Y, MinBounds.Y, MaxBounds.Y);
        Pos.Z = FMath::Clamp(Pos.Z, MinBounds.Z, MaxBounds.Z);

        Parcel->SetActorLocation(Pos);
    }
}

void AFluidSimVolume::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Draw debug bounding box
    if (bDrawDebugBox)
    {
        DrawDebugBox(GetWorld(), GetActorLocation(), VolumeHalfExtents * GetActorScale(), FColor::Green, false, -1.f, 0, 2.f);
    }

    // Compute dynamic smoothing radius
    FVector VolumeSize = VolumeHalfExtents * 2.f * GetActorScale();
    float VolumeTotal = VolumeSize.X * VolumeSize.Y * VolumeSize.Z;
    float ParticleSpacing = FMath::Pow(VolumeTotal / FMath::Max(NumParcels, 1), 1.f / 3.f);
    float h = ParticleSpacing * 1.1f; // smoothing radius

    // --- Simulation substepping for stability ---
    const int32 Substeps = 32;
    const float SubDeltaTime = DeltaTime / Substeps;

    for (int32 Step = 0; Step < Substeps; Step++)
    {
        for (AFluidParcelActor* PiActor : Parcels)
        {
            UFluidParcel* Pi = PiActor->Parcel;
            if (!Pi || Pi->bIsBoundary) continue; // skip boundary particles for integration

            Pi->Density = 0.f;

            for (AFluidParcelActor* PjActor : Parcels)
            {
                UFluidParcel* Pj = PjActor->Parcel;
                if (!Pj) continue;

                FVector Rij = Pi->Position - Pj->Position;
                float r = Rij.Size();
                if (r < h && r > 0.f)
                {
                    Pi->Density += Pj->Mass * SPH::Poly6(r, h);
                }
            }

            Pi->Density = FMath::Max(Pi->Density, 1e-3f);
            Pi->Pressure = GasConstant * (FMath::Pow(Pi->Density / RestDensity, 7.f) - 1.f); // Tait equation of state
        }

        // --- Compute forces and integrate ---
        const float MaxForce = 5000.f; // Clamp force magnitude
        const float ArtificialDamping = 0.995f;

        for (AFluidParcelActor* PiActor : Parcels)
        {
            UFluidParcel* Pi = PiActor->Parcel;
            if (!Pi || Pi->bIsBoundary) continue;

            FVector PressureForce = FVector::ZeroVector;
            FVector ViscosityForce = FVector::ZeroVector;

            for (AFluidParcelActor* PjActor : Parcels)
            {
                UFluidParcel* Pj = PjActor->Parcel;
                if (!Pj || Pi == Pj) continue;

                FVector Rij = Pi->Position - Pj->Position;
                float r = Rij.Size();
                if (r < h && r > 0.f)
                {
                    FVector gradW = SPH::SpikyGradient(Rij, h);
                    float PressureTerm = (Pi->Pressure / (Pi->Density * Pi->Density)) +
                        (Pj->Pressure / (Pj->Density * Pj->Density));

                    PressureForce -= Pj->Mass * PressureTerm * gradW;

                    float lap = SPH::ViscosityLaplacian(r, h);
                    ViscosityForce += Viscosity * Pj->Mass * (Pj->Velocity - Pi->Velocity) / Pj->Density * lap;
                }
            }

            FVector TotalForce = PressureForce + ViscosityForce + FVector(0.f, 0.f, Gravity) * Pi->Mass;
            TotalForce = TotalForce.GetClampedToMaxSize(5000.f);

            FVector Acceleration = TotalForce / Pi->Mass;
            Pi->Velocity += Acceleration * SubDeltaTime;
            Pi->Position += Pi->Velocity * SubDeltaTime;
        }

        VolumeCollisionCheck(SubDeltaTime);
    }

}