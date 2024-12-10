using UnrealBuildTool;

public class LabInteractionEditor : ModuleRules
{
    public LabInteractionEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore", 
                "LabInteraction",
                "AssetTools", 
                "UnrealEd"
            }
        );
    }
}