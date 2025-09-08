using UnrealBuildTool;

public class PBFluidSimEditorTarget : TargetRules
{
    public PBFluidSimEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        CppStandard = CppStandardVersion.Latest;
        ExtraModuleNames.AddRange( new string[] { "PBFluidSimCore" } );
    }
}