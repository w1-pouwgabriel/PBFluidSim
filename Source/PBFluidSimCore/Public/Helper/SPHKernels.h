#pragma once

#include "CoreMinimal.h"

// Constants
static constexpr float PI_F = 3.14159265358979323846f;

// Compute common coefficients for given h (support radius)
struct FSPHKernelCoeffs
{
    float H;
    float H2;
    float H3;
    float H6;
    float H9;

    float Poly6Coeff;     // for density (poly6)
    float SpikyGradCoeff; // for pressure gradient (spiky)
    float ViscLaplacianCoeff; // for viscosity (laplacian)

    explicit FSPHKernelCoeffs(float InH = 100.0f) // default support radius
    {
        H = InH;
        H2 = H * H;
        H3 = H2 * H;
        H6 = H3 * H3;
        H9 = H6 * H3;

        // 3D normalization factors:
        // poly6: 315 / (64 * PI * h^9)
        // spiky grad: -45 / (PI * h^6)
        // viscosity laplacian: 45 / (PI * h^6)
        Poly6Coeff = 315.0f / (64.0f * PI_F * H9);
        SpikyGradCoeff = -45.0f / (PI_F * H6);
        ViscLaplacianCoeff = 45.0f / (PI_F * H6);
    }
};

// Poly6 kernel (density). Accepts r^2 for efficiency.
FORCEINLINE float KernelPoly6_SqDist(float r2, const FSPHKernelCoeffs& C)
{
    if (r2 < 0.0f || r2 > C.H2) return 0.0f;
    float diff = (C.H2 - r2);
    // (h^2 - r^2)^3
    return C.Poly6Coeff * diff * diff * diff;
}

// Spiky kernel gradient (for pressure). Returns vector pointing from i->j multiplied by scalar gradient magnitude.
// We pass actual distance r and the direction (ri_to_rj) normalized or not?
// We'll calculate using ri_to_rj vector.
FORCEINLINE FVector KernelSpikyGradient(const FVector& rij, float r, const FSPHKernelCoeffs& C)
{
    if (r <= 0.0f || r > C.H) return FVector::ZeroVector;

    float x = (C.H - r);
    // magnitude: -45 / (pi h^6) * (h - r)^2
    float mag = C.SpikyGradCoeff * x * x / (r); // divide by r because we need gradient vector = mag * (rij)
    // note: SpikyGradCoeff is negative already as written above, so mag will be negative; we will use formula with (p_i + p_j)
    return rij * mag;
}

// Viscosity Laplacian (scalar)
FORCEINLINE float KernelViscLaplacian(float r, const FSPHKernelCoeffs& C)
{
    if (r < 0.0f || r > C.H) return 0.0f;
    return C.ViscLaplacianCoeff * (C.H - r);
}
