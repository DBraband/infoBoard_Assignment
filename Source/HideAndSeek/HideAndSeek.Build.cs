// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HideAndSeek : ModuleRules
{
	public HideAndSeek(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"Slate",
			"SlateCore",
			"UMG",
			"HeadMountedDisplay",
			"NavigationSystem",
			"AIModule",
			"GameplayTasks",
			"Niagara",
			"EnhancedInput",
		});
	}
}