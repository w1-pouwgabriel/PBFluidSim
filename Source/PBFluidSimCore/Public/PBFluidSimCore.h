#pragma once
    
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class PBFluidSimCore : public IModuleInterface
{
public:
    static inline PBFluidSimCore& Get()
    {
        return FModuleManager::LoadModuleChecked<PBFluidSimCore>("PBFluidSimCore");
    }

    static inline bool IsAvailable()
    {
        return FModuleManager::Get().IsModuleLoaded("PBFluidSimCore");
    }

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};