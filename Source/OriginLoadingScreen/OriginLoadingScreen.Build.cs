// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OriginLoadingScreen : ModuleRules
{
	public OriginLoadingScreen(ReadOnlyTargetRules Target)
		: base(Target)
	{

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PrivatePCHHeaderFile = "Public/OriginLoadingScreen.h";

		PrivateIncludePaths.Add("OriginLoadingScreen/Private");

		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"MoviePlayer",
				"Slate",
				"SlateCore",
				"InputCore", "UMG"
			}
		);
	}
}