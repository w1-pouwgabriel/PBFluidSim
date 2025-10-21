// FluidSimVolume.cpp
// ------------------------------------------------------------
// CPU-based SPH (Smoothed Particle Hydrodynamics) simulation
// Reference: https://en.wikipedia.org/wiki/Smoothed-particle_hydrodynamics
// ------------------------------------------------------------

#include "FluidSimVolume.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Helper/SPHMath.h"

// ------------------------------------------------------------
// Constructor: sets up basic parameters for the fluid volume
// ------------------------------------------------------------
AFluidSimVolume::AFluidSimVolume()
    : VolumeHalfExtents(FVector(250.f, 250.f, 250.f)), NumParcels(25), bDrawDebugBox(true)
{
    PrimaryActorTick.bCanEverTick = true;
}

// ------------------------------------------------------------
// BeginPlay: initializes particle positions and smoothing radius
// ------------------------------------------------------------
void AFluidSimVolume::BeginPlay()
{
    Super::BeginPlay();

    int32 NumPerAxis = FMath::RoundToInt(FMath::Pow((float)NumParcels, 1.f / 3.f));
    NumPerAxis = FMath::Max(1, NumPerAxis);

    FVector VolumeSize = VolumeHalfExtents * 2.f * GetActorScale();
    float ParticleSpacing = FMath::Min3(
        VolumeSize.X / NumPerAxis,
        VolumeSize.Y / NumPerAxis,
        VolumeSize.Z / NumPerAxis
    );

    FVector VolumeOrigin = GetActorLocation();
    FVector StartPos = VolumeOrigin - VolumeHalfExtents + FVector(ParticleSpacing * 0.5f);

    int32 ParticleCount = 0;
    float ParticleMass = 2.f;

    // Larger h -> more neighbors for density/pressure
    SmoothingRadius = ParticleSpacing * 1.8f;

    // Generate fluid parcels in a uniform grid
    for (int32 ix = 0; ix < NumPerAxis; ix++)
    {
        for (int32 iy = 0; iy < NumPerAxis; iy++)
        {
            for (int32 iz = 0; iz < NumPerAxis; iz++)
            {
                if (ParticleCount >= NumParcels) break;

                FVector Pos = StartPos + FVector(ix, iy, iz) * ParticleSpacing;
                FVector Vel = FVector::ZeroVector;

                AFluidParcelActor* ParcelActor = GetWorld()->SpawnActor<AFluidParcelActor>(Pos, FRotator::ZeroRotator);
                if (ParcelActor)
                {
                    ParcelActor->Parcel->Position = Pos;
                    ParcelActor->Parcel->Velocity = Vel;
                    ParcelActor->Parcel->Mass = ParticleMass;

                    // Initialize density to RestDensity to prevent initial explosion
                    ParcelActor->Parcel->Density = RestDensity > 0.f ? RestDensity : 1.0f;
                    Parcels.Add(ParcelActor);
                }
                ParticleCount++;
            }
        }
    }
}

// ------------------------------------------------------------
// ComputeWallRepulsion: soft cubic wall repulsion helper
// Reference: Müller et al. (2003) - Particle-Based Fluid Simulation for Interactive Applications
// ------------------------------------------------------------
inline float ComputeWallRepulsion(float Distance, float MinDist, float k)
{
    if (Distance < MinDist)
    {
        float ratio = 1.f - Distance / MinDist; // 0 at edge, 1 at wall
        return k * ratio * ratio * ratio;       // cubic soft push
    }
    return 0.f;
}

// ------------------------------------------------------------
// VolumeCollisionCheck: applies soft boundary constraints to particles
// ------------------------------------------------------------
void AFluidSimVolume::VolumeCollisionCheck(float DeltaTime)
{
    if (DeltaTime <= 0.f) return;

    // Wall constraint parameters tuned for SPH
    const float WallBaseStrength = 60.f;
    const float MinWallDist = SmoothingRadius;
    const float MaxWallForce = 1000.f;
    const float WallDamping = 0.4f;

    FVector MinBounds = GetActorLocation() - VolumeHalfExtents * GetActorScale();
    FVector MaxBounds = GetActorLocation() + VolumeHalfExtents * GetActorScale();

    for (AFluidParcelActor* ParcelActor : Parcels)
    {
        if (!ParcelActor || !ParcelActor->Parcel) continue;

        UFluidParcel* P = ParcelActor->Parcel;
        FVector& Pos = P->Position;
        FVector& Vel = P->Velocity;

        FVector WallForce = FVector::ZeroVector;

        // X-axis walls
        float DistMinX = Pos.X - MinBounds.X;
        if (DistMinX < MinWallDist)
            WallForce.X += WallBaseStrength * FMath::Pow(MinWallDist - DistMinX, 2);

        float DistMaxX = MaxBounds.X - Pos.X;
        if (DistMaxX < MinWallDist)
            WallForce.X -= WallBaseStrength * FMath::Pow(MinWallDist - DistMaxX, 2);

        // Y-axis walls
        float DistMinY = Pos.Y - MinBounds.Y;
        if (DistMinY < MinWallDist)
            WallForce.Y += WallBaseStrength * FMath::Pow(MinWallDist - DistMinY, 2);

        float DistMaxY = MaxBounds.Y - Pos.Y;
        if (DistMaxY < MinWallDist)
            WallForce.Y -= WallBaseStrength * FMath::Pow(MinWallDist - DistMaxY, 2);

        // Z-axis walls (floor and ceiling)
        float DistMinZ = Pos.Z - MinBounds.Z;
        if (DistMinZ < MinWallDist)
            WallForce.Z += WallBaseStrength * FMath::Pow(MinWallDist - DistMinZ, 2);

        float DistMaxZ = MaxBounds.Z - Pos.Z;
        if (DistMaxZ < MinWallDist)
            WallForce.Z -= WallBaseStrength * FMath::Pow(MinWallDist - DistMaxZ, 2);

        // Clamp to prevent instability
        WallForce = WallForce.GetClampedToMaxSize(MaxWallForce);

        // Apply the repulsion
        Vel += (WallForce / FMath::Max(P->Mass, 1e-6f)) * DeltaTime;

        // Damp velocity along walls to prevent jitter
        FVector WallNormal(
            (DistMinX < MinWallDist) ? 1.f : (DistMaxX < MinWallDist ? -1.f : 0.f),
            (DistMinY < MinWallDist) ? 1.f : (DistMaxY < MinWallDist ? -1.f : 0.f),
            (DistMinZ < MinWallDist) ? 1.f : (DistMaxZ < MinWallDist ? -1.f : 0.f)
        );

        if (!WallNormal.IsZero())
        {
            WallNormal.Normalize();
            float Dot = FVector::DotProduct(Vel, WallNormal);
            if (Dot < 0.f)
                Vel -= WallNormal * Dot * WallDamping;
        }

        // Prevent tunneling below floor
        float FloorZ = MinBounds.Z;
        if (Pos.Z < FloorZ)
        {
            Pos.Z = FloorZ;
            if (Vel.Z < 0.f) Vel.Z = 0.f;
        }
    }
}

// ------------------------------------------------------------
// Tick: Main SPH simulation loop
// ------------------------------------------------------------
void AFluidSimVolume::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bDrawDebugBox)
    {
        DrawDebugBox(GetWorld(), GetActorLocation(), VolumeHalfExtents * GetActorScale(), FColor::Green, false, -1.f, 0, 2.f);
    }

    // Recompute spacing and smoothing radius consistency
    FVector VolumeSize = VolumeHalfExtents * 2.f * GetActorScale();
    float VolumeTotal = VolumeSize.X * VolumeSize.Y * VolumeSize.Z;
    float ParticleSpacing = FMath::Pow(VolumeTotal / FMath::Max(NumParcels, 1), 1.f / 3.f);

    float h = SmoothingRadius > 0.f ? SmoothingRadius : ParticleSpacing * 1.5f;

    // --------------------------------------------------------
    // 1. Compute density and pressure
    // --------------------------------------------------------
    for (AFluidParcelActor* PiActor : Parcels)
    {
        UFluidParcel* Pi = PiActor->Parcel;
        if (!Pi || Pi->bIsBoundary) continue;
        Pi->Density = 0.f;
    }

    for (AFluidParcelActor* PiActor : Parcels)
    {
        UFluidParcel* Pi = PiActor->Parcel;
        if (!Pi || Pi->bIsBoundary) continue;

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

        // Equation of state: P = k * (rho - rho0)
        // Reference: Müller et al. (2003)
        const float PressureStiffness = FMath::Max(1.f, GasConstant);
        Pi->Pressure = PressureStiffness * (Pi->Density - RestDensity);
    }

    // --------------------------------------------------------
    // 2. Compute forces (pressure, viscosity, gravity)
    // --------------------------------------------------------
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

                // Symmetric pressure term
                float PressureTerm = (Pi->Pressure / (Pi->Density * Pi->Density)) +
                                     (Pj->Pressure / (Pj->Density * Pj->Density));
                PressureForce -= 0.5f * Pj->Mass * PressureTerm * gradW;

                // Viscosity term (Laplacian)
                float lap = SPH::ViscosityLaplacian(r, h);
                ViscosityForce += Viscosity * Pj->Mass * (Pj->Velocity - Pi->Velocity) / Pj->Density * lap;
            }
        }

        // Total external + internal forces
        FVector TotalForce = PressureForce + ViscosityForce + FVector(0.f, 0.f, Gravity) * Pi->Mass;
        TotalForce = TotalForce.GetClampedToMaxSize(5000.f);

        FVector Acceleration = TotalForce / Pi->Mass;
        Pi->Velocity += Acceleration * DeltaTime;
        Pi->Position += Pi->Velocity * DeltaTime;
    }

    // --------------------------------------------------------
    // 3. Boundary checks
    // --------------------------------------------------------
    VolumeCollisionCheck(DeltaTime);

    // --------------------------------------------------------
    // 4. XSPH velocity smoothing
    // Reference: Monaghan (1992) - XSPH correction
    // --------------------------------------------------------
    const float xsph_epsilon = 0.05f;
    for (AFluidParcelActor* PiActor : Parcels)
    {
        UFluidParcel* Pi = PiActor->Parcel;
        if (!Pi || Pi->bIsBoundary) continue;

        FVector vsum = FVector::ZeroVector;
        float wsum = 0.f;
        for (AFluidParcelActor* PjActor : Parcels)
        {
            UFluidParcel* Pj = PjActor->Parcel;
            if (!Pj || Pi == Pj) continue;

            FVector Rij = Pi->Position - Pj->Position;
            float r = Rij.Size();
            if (r < h && r > 0.f)
            {
                float w = SPH::Poly6(r, h);
                vsum += Pj->Velocity * w;
                wsum += w;
            }
        }
        if (wsum > 0.f)
        {
            Pi->Velocity += xsph_epsilon * ((vsum / wsum) - Pi->Velocity);
        }
    }

    // --------------------------------------------------------
    // 5. Update Unreal actor transforms
    // --------------------------------------------------------
    for (AFluidParcelActor* ParcelActor : Parcels)
    {
        if (!ParcelActor || !ParcelActor->Parcel) continue;
        ParcelActor->SetActorLocation(ParcelActor->Parcel->Position);
    }
}
