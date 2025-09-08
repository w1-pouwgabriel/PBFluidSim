using UnrealBuildTool;
    
public class PBFluidSimTarget : TargetRules
{
    public PBFluidSimTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        CppStandard = CppStandardVersion.Latest;
        ExtraModuleNames.AddRange( new string[] { "PBFluidSimCore" } );
    }
}