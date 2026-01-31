// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Marine_Monopolist : ModuleRules
{
	public Marine_Monopolist(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"NavigationSystem",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"Niagara",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Marine_Monopolist",
			"Marine_Monopolist/Variant_Strategy",
			"Marine_Monopolist/Variant_Strategy/UI",
			"Marine_Monopolist/Variant_TwinStick",
			"Marine_Monopolist/Variant_TwinStick/AI",
			"Marine_Monopolist/Variant_TwinStick/Gameplay",
			"Marine_Monopolist/Variant_TwinStick/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
