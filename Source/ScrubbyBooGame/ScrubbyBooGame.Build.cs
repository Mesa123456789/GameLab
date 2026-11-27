// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ScrubbyBooGame : ModuleRules
{
	public ScrubbyBooGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"NetCore",
			"OnlineSubsystem",
			"OnlineSubsystemUtils"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"ScrubbyBooGame",
			"ScrubbyBooGame/Variant_Horror",
			"ScrubbyBooGame/Variant_Horror/UI",
			"ScrubbyBooGame/Variant_Shooter",
			"ScrubbyBooGame/Variant_Shooter/AI",
			"ScrubbyBooGame/Variant_Shooter/UI",
			"ScrubbyBooGame/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
