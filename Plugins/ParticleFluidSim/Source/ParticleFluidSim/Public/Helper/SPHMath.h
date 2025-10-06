#pragma once

#include "CoreMinimal.h"

namespace SPH
{
    inline float Poly6(float r, float h)
    {
        if (r >= 0 && r <= h)
        {
            float term = (h * h - r * r);
            return (315.f / (64.f * PI * pow(h, 9))) * term * term * term;
        }
        return 0.f;
    }

    inline FVector SpikyGradient(const FVector& rVec, float h)
    {
        float r = rVec.Size();
        if (r > 0 && r <= h)
        {
            float coeff = -45.f / (PI * pow(h, 6)) * pow(h - r, 2);
            return coeff * (rVec / r);
        }
        return FVector::ZeroVector;
    }

    inline float ViscosityLaplacian(float r, float h)
    {
        if (r >= 0 && r <= h)
        {
            return 45.f / (PI * pow(h, 6)) * (h - r);
        }
        return 0.f;
    }
}
