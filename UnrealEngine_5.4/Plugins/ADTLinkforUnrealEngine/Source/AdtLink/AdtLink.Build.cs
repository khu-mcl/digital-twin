//////////////////////////////////////////////////////
// Copyright (C) WSP. 2021. All rights reserved.
//////////////////////////////////////////////////////

using UnrealBuildTool;

public class AdtLink : ModuleRules
{
	public AdtLink(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {

			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {

			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"WebSockets",
				"HTTP",
				"Projects"
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Json",
                "JsonUtilities",
				//Editor only module
#if WITH_EDITOR
				"DesktopPlatform"
#endif
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
