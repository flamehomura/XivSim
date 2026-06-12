// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class XivSimEditorTarget : TargetRules
{
	public XivSimEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		BuildEnvironment = TargetBuildEnvironment.Unique;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange(new string[] { "XivSim" });
		if (!bBuildAllModules)
		{
			NativePointerMemberBehaviorOverride = PointerMemberBehavior.Disallow;
		}
        XivSimTarget.ApplySharedXivSimSettings(this);
	}
}
