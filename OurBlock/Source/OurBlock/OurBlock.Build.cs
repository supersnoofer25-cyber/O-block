using UnrealBuildTool;

public class OurBlock : ModuleRules
{
	public OurBlock(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
		});

		// Campaign/*.cpp (see Campaign/Campaign.h) are shims that pull cpp/campaign
		// directly into this module - it was written and tested standalone with plain
		// clang++ assuming ordinary C++ exception behaviour (persist.cpp's std::stoll
		// can throw), which UE's default module settings don't provide.
		CppStandard = CppStandardVersion.Cpp20;
		bEnableExceptions = true;
	}
}
