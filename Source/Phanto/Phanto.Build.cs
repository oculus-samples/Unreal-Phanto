// Copyright (c) Meta Platforms, Inc. and affiliates.

using UnrealBuildTool;

public class Phanto : ModuleRules
{
	public Phanto(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "OculusXRHMD", "OculusXRScene", "OculusXRAnchors", "AIModule" });
        
        // Required for OpenXR support
        PublicIncludePathModuleNames.AddRange(new string[] { "OpenXRHMD" });

        PrivateIncludePaths.Add($"{GetModuleDirectory("OculusXRScene")}/Public");
        PrivateIncludePaths.Add($"{GetModuleDirectory("OculusXRAnchors")}/Public");
        PrivateIncludePaths.Add($"{GetModuleDirectory("OculusXRHMD")}/Public");

        //PrivateIncludePathModuleNames.AddRange(new string[] { "NavigationSystem" });

        //      PrivateDependencyModuleNames.AddRange(new string[] { "NavigationSystem" });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
