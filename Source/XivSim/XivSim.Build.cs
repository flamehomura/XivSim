// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class XivSim : ModuleRules
{
	public XivSim(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
          new string[] { 
              "Core", 
              "NetCore", 
              "CoreUObject", 
              "Engine", 
              "InputCore", 
              "HeadMountedDisplay", 
              "EnhancedInput", 
              "Niagara",
              "UMG",
              "Slate",
              "SlateCore",
              "AIModule"
          });
        PublicIncludePaths.AddRange(
          new string[] {
            "XivSim",
            "XivSim/Player",
            "XivSim/Gameplay",
            "XivSim/Buff",
            "XivSim/UI",
            "XivSim/Manager",
            "XivSim/Pattern",
            "XivSim/Action",
          }
        );
    }
}
