// Editor target, so the project can actually be opened and worked on in-editor.
using UnrealBuildTool;
using System.Collections.Generic;

public class OurBlockEditorTarget : TargetRules
{
	public OurBlockEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange(new string[] { "OurBlock" });
	}
}
