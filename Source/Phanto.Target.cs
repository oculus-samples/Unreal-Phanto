// Copyright (c) Meta Platforms, Inc. and affiliates.

using UnrealBuildTool;
using System.Collections.Generic;

public class PhantoTarget : TargetRules
{
	public PhantoTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;

		ExtraModuleNames.AddRange( new string[] { "Phanto" } );
	}
}
