// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Territoru : ModuleRules
{
	public Territoru(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG" });
	}
}
