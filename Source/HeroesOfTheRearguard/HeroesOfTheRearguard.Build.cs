// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HeroesOfTheRearguard : ModuleRules
{
	public HeroesOfTheRearguard(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTasks",
			"GameplayTags",
			"UMG" ,
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "AIModule" });
	}
}
