using UnrealBuildTool;

public class PBFluidSimCore : ModuleRules
{
    public PBFluidSimCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine"
        });
        
        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}

