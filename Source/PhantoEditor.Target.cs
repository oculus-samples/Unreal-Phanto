// Copyright (c) Meta Platforms, Inc. and affiliates.

using UnrealBuildTool;
using System.Collections.Generic;

public class PhantoEditorTarget : TargetRules
{
	public PhantoEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;

		ExtraModuleNames.AddRange( new string[] { "Phanto" } );
	}
}
