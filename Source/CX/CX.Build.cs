
using UnrealBuildTool;

public class CX : ModuleRules
{
    public CX(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "ChaosVehicles",
            "Niagara"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
        });
    }
}
