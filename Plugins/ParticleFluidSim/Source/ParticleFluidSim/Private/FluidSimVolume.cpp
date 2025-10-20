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

    SmoothingRadius = ParticleSpacing * 1.8f; // larger h -> more neighbors

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
                    // initialize density to something near rest density so pressure doesn't explode
                    ParcelActor->Parcel->Density = RestDensity > 0.f ? RestDensity : 1.0f;
                    Parcels.Add(ParcelActor);
                }
                ParticleCount++;
            }
        }
    }
}

inline float ComputeWallRepulsion(float Distance, float MinDist, float k)
{
    if (Distance < MinDist)
    {
        float ratio = 1.f - Distance / MinDist; // 0 at edge, 1 at wall
        return k * ratio * ratio * ratio;       // cubic soft push
    }
    return 0.f;
}

void AFluidSimVolume::VolumeCollisionCheck(float DeltaTime)
{
    if (DeltaTime <= 0.f) return;

    // --- Tuned SPH-friendly wall parameters ---
    const float WallBaseStrength = 60.f;
    const float MinWallDist = SmoothingRadius;
    const float MaxWallForce = 1000.f;           // clamp to avoid explosions
    const float WallDamping = 0.4f;              // reduce velocity into wall

    FVector MinBounds = GetActorLocation() - VolumeHalfExtents * GetActorScale();
    FVector MaxBounds = GetActorLocation() + VolumeHalfExtents * GetActorScale();

    for (AFluidParcelActor* ParcelActor : Parcels)
    {
        if (!ParcelActor || !ParcelActor->Parcel) continue;

        UFluidParcel* P = ParcelActor->Parcel;
        FVector& Pos = P->Position;
        FVector& Vel = P->Velocity;

        FVector WallForce = FVector::ZeroVector;

        // --- X-axis ---
        float DistMinX = Pos.X - MinBounds.X;
        if (DistMinX < MinWallDist)
            WallForce.X += WallBaseStrength * FMath::Pow(MinWallDist - DistMinX, 2);

        float DistMaxX = MaxBounds.X - Pos.X;
        if (DistMaxX < MinWallDist)
            WallForce.X -= WallBaseStrength * FMath::Pow(MinWallDist - DistMaxX, 2);

        // --- Y-axis ---
        float DistMinY = Pos.Y - MinBounds.Y;
        if (DistMinY < MinWallDist)
            WallForce.Y += WallBaseStrength * FMath::Pow(MinWallDist - DistMinY, 2);

        float DistMaxY = MaxBounds.Y - Pos.Y;
        if (DistMaxY < MinWallDist)
            WallForce.Y -= WallBaseStrength * FMath::Pow(MinWallDist - DistMaxY, 2);

        // --- Z-axis (floor & ceiling) ---
        float DistMinZ = Pos.Z - MinBounds.Z;
        if (DistMinZ < MinWallDist)
            WallForce.Z += WallBaseStrength * FMath::Pow(MinWallDist - DistMinZ, 2);

        float DistMaxZ = MaxBounds.Z - Pos.Z;
        if (DistMaxZ < MinWallDist)
            WallForce.Z -= WallBaseStrength * FMath::Pow(MinWallDist - DistMaxZ, 2);

        // Clamp the force for stability
        WallForce = WallForce.GetClampedToMaxSize(MaxWallForce);

        // Apply soft wall force
        Vel += (WallForce / FMath::Max(P->Mass, 1e-6f)) * DeltaTime;

        // --- Velocity damping along the wall ---
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

        float FloorZ = MinBounds.Z;
        if (Pos.Z < FloorZ)
        {
            Pos.Z = FloorZ;
            if (Vel.Z < 0.f) Vel.Z = 0.f; // stop downward velocity
        }
    }
}




void AFluidSimVolume::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bDrawDebugBox)
    {
        DrawDebugBox(GetWorld(), GetActorLocation(), VolumeHalfExtents * GetActorScale(), FColor::Green, false, -1.f, 0, 2.f);
    }

    // Compute dynamic particle spacing + smoothing radius consistency
    FVector VolumeSize = VolumeHalfExtents * 2.f * GetActorScale();
    float VolumeTotal = VolumeSize.X * VolumeSize.Y * VolumeSize.Z;
    float ParticleSpacing = FMath::Pow(VolumeTotal / FMath::Max(NumParcels, 1), 1.f / 3.f);

    // Use SmoothingRadius (set at BeginPlay) but if you want to recompute:
    float h = SmoothingRadius > 0.f ? SmoothingRadius : ParticleSpacing * 1.5f;


    // --- 1 Density & pressure ---
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

        const float PressureStiffness = FMath::Max(1.f, GasConstant); // use GasConstant as k
        Pi->Pressure = PressureStiffness * (Pi->Density - RestDensity);
    }

    // --- 2 Forces (pressure + viscosity + gravity) and integrate velocity/position ---
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
                PressureForce -= 0.5f * Pj->Mass * PressureTerm * gradW;


                float lap = SPH::ViscosityLaplacian(r, h);
                ViscosityForce += Viscosity * Pj->Mass * (Pj->Velocity - Pi->Velocity) / Pj->Density * lap;
            }
        }

        FVector TotalForce = PressureForce + ViscosityForce + FVector(0.f, 0.f, Gravity) * Pi->Mass;
        TotalForce = TotalForce.GetClampedToMaxSize(5000.f);

        FVector Acceleration = TotalForce / Pi->Mass;
        Pi->Velocity += Acceleration * DeltaTime; 
        Pi->Position += Pi->Velocity * DeltaTime;
    }

    // --- 3 Boundary check ---
    VolumeCollisionCheck(DeltaTime);

    // --- 4 XSPH velocity smoothing to reduce noise ---
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
        

    // --- 5 Update actor transforms ---
    for (AFluidParcelActor* ParcelActor : Parcels)
    {
        if (!ParcelActor || !ParcelActor->Parcel) continue;
        ParcelActor->SetActorLocation(ParcelActor->Parcel->Position);
    }
}
