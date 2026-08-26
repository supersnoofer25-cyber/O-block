// Game target: what ships. See OurBlockEditor.Target.cs for the editor build.
using UnrealBuildTool;
using System.Collections.Generic;

public class OurBlockTarget : TargetRules
{
	public OurBlockTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange(new string[] { "OurBlock" });
	}
}
