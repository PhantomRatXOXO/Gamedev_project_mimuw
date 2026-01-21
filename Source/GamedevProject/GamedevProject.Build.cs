// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GamedevProject : ModuleRules
{
	public GamedevProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"EnhancedInput",
				"Niagara",
				"AIModule",
				"NavigationSystem"
			}
		);

		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}

