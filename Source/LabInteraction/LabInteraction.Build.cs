// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LabInteraction : ModuleRules
{
	public LabInteraction(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"UMG"
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
			"EnhancedInput",
		});
	}
}
