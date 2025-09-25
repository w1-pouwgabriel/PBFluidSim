// FluidSimVolume.cpp
#include "FluidSimVolume.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

AFluidSimVolume::AFluidSimVolume()
{
    PrimaryActorTick.bCanEverTick = true;
    VolumeHalfExtents = FVector(250.f, 250.f, 250.f);
    NumParcels = 50;
    bDrawDebugBox = true;
}

void AFluidSimVolume::BeginPlay()
{
    Super::BeginPlay();

    for (int32 i = 0; i < NumParcels; i++)
    {
        FVector VolumeOrigin = GetActorLocation();

        FVector MinBounds = GetActorLocation() - VolumeHalfExtents;
        FVector MaxBounds = GetActorLocation() + VolumeHalfExtents;

        FVector RandPos = VolumeOrigin +
            FVector(
                FMath::FRandRange(-VolumeHalfExtents.X, VolumeHalfExtents.X),
                FMath::FRandRange(-VolumeHalfExtents.Y, VolumeHalfExtents.Y),
                FMath::FRandRange(-VolumeHalfExtents.Z, VolumeHalfExtents.Z)
            );

        FVector RandVel = FVector(FMath::FRandRange(-50.f, 50.f),
                                  FMath::FRandRange(-50.f, 50.f),
                                  FMath::FRandRange(-50.f, 50.f));

        AFluidParcelActor* ParcelActor = GetWorld()->SpawnActor<AFluidParcelActor>(RandPos, FRotator::ZeroRotator);
        if (ParcelActor)
        {
            ParcelActor->InitParcel(RandPos, RandVel);
            Parcels.Add(ParcelActor);
        }
    }
}

void AFluidSimVolume::HandleParticleCollisions()
{
    const float ParticleRadius = 10.f; // treat each parcel as a sphere
    const float Stiffness = 200.f;     // how strongly they push each other

    for (int32 i = 0; i < Parcels.Num(); i++)
    {
        for (int32 j = i + 1; j < Parcels.Num(); j++)
        {
            AFluidParcelActor* A = Parcels[i];
            AFluidParcelActor* B = Parcels[j];
            if (!A || !B || !A->Parcel || !B->Parcel) continue;

            FVector Dir = B->Parcel->Position - A->Parcel->Position;
            float Dist = Dir.Size();

            if (Dist > KINDA_SMALL_NUMBER && Dist < 2 * ParticleRadius)
            {
                // Normalize
                FVector Normal = Dir / Dist;

                // Overlap amount
                float Penetration = 2 * ParticleRadius - Dist;

                // Apply simple spring force (repulsion)
                FVector Force = Normal * (Penetration * Stiffness);

                // Push particles apart
                A->Parcel->Velocity -= Force / A->Parcel->Mass;
                B->Parcel->Velocity += Force / B->Parcel->Mass;
            }
        }
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

    // Check collisions against volume bounds
    for (AFluidParcelActor* Parcel : Parcels)
    {
        if (!Parcel || !Parcel->Parcel) continue;

        FVector& Pos = Parcel->Parcel->Position;
        FVector& Vel = Parcel->Parcel->Velocity;

        FVector MinBounds = GetActorLocation() - VolumeHalfExtents;
        FVector MaxBounds = GetActorLocation() + VolumeHalfExtents;

        // X axis
        if (Pos.X < MinBounds.X) { Pos.X = MinBounds.X; Vel.X *= -1.f; }
        else if (Pos.X > MaxBounds.X) { Pos.X = MaxBounds.X; Vel.X *= -1.f; }

        // Y axis
        if (Pos.Y < MinBounds.Y) { Pos.Y = MinBounds.Y; Vel.Y *= -1.f; }
        else if (Pos.Y > MaxBounds.Y) { Pos.Y = MaxBounds.Y; Vel.Y *= -1.f; }

        // Z axis
        if (Pos.Z < MinBounds.Z) { Pos.Z = MinBounds.Z; Vel.Z *= -1.f; }
        else if (Pos.Z > MaxBounds.Z) { Pos.Z = MaxBounds.Z; Vel.Z *= -1.f; }

        // Apply updated position
        Parcel->SetActorLocation(Pos);
    }

    HandleParticleCollisions();

    // Apply new positions
    for (AFluidParcelActor* ParcelActor : Parcels)
    {
        if (ParcelActor && ParcelActor->Parcel)
        {
            ParcelActor->SetActorLocation(ParcelActor->Parcel->Position);
        }
    }
}
