// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UC02CH03P02 : ModuleRules
{
	public UC02CH03P02(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
	}
}
