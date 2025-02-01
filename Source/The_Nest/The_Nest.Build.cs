// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class The_Nest : ModuleRules
{
	public The_Nest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "Slate", "EnhancedInput","SlateCore", "EngineSettings" });

		PrivateDependencyModuleNames.AddRange(new string[] { "GameplayTags", "DeveloperSettings", "OnlineSubsystem", "OnlineSubsystemSteam" });
		
	}
	
}
