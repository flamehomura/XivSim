// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class XivSimServerTarget : TargetRules
{
    public XivSimServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        ExtraModuleNames.AddRange(new string[] { "XivSim" });
        XivSimTarget.ApplySharedXivSimSettings(this);
        bUseChecksInShipping = true;
    }
}