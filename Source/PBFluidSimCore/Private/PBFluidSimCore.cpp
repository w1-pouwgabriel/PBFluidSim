#include "PBFluidSimCore.h"
#include "Modules/ModuleManager.h"

#include "Log.h"

void PBFluidSimCore::StartupModule()
{
    UE_LOG(LogPBFluidSimCore, Log, TEXT("PBFluidSimCore module starting up"));
}

void PBFluidSimCore::ShutdownModule()
{
    UE_LOG(LogPBFluidSimCore, Log, TEXT("PBFluidSimCore module shutting down"));
}

IMPLEMENT_PRIMARY_GAME_MODULE(PBFluidSimCore, PBFluidSimCore, "PBFluidSimCore");