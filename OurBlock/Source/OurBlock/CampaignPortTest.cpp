// Proves the shims under Campaign/ (see Campaign/Campaign.h) actually link and produce
// correct results once compiled by MSVC under UE's own build settings - a different
// compiler and flags entirely from the clang++ build cpp/campaign_test.cpp already
// covers, which is ADR 0014's specification. Not a replacement for that suite; a check
// that the port survived crossing into this toolchain too, run the same headless way:
//
//   UnrealEditor-Cmd.exe OurBlock.uproject -ExecCmds="Automation RunTests OurBlock.Campaign; Quit" -unattended -nopause -nullrhi
//
// UCampaignSubsystem itself isn't exercised here - it needs a real UGameInstance,
// which needs a PIE session, which needs a level, none of which exist yet (see
// CLAUDE.md's next decision). This tests the layer underneath it that's actually new
// and at risk: whether the ported rules module survives being compiled here at all,
// which is exactly what failed twice while wiring this in.
#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Campaign/Campaign.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCampaignPortSmokeTest, "OurBlock.Campaign.PortCompilesAndRuns",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FCampaignPortSmokeTest::RunTest(const FString& Parameters)
{
	campaign::State S = campaign::New();
	if (!TestEqual(TEXT("New() starts with all eight alive"), static_cast<int32>(S.Alive.size()), 8))
	{
		return false;
	}

	campaign::BipOut Decision;
	Decision.Seat = campaign::Seat::Riding;
	Decision.Companion = campaign::Tyrone;
	Decision.CompanionReturned = false;

	campaign::Result Result = campaign::Apply(S, Decision);
	if (!TestTrue(TEXT("a legal BipOut is not an error"), Result.Ok()))
	{
		return false;
	}
	if (!TestFalse(TEXT("Tyrone is not alive after not returning"), Result.S.IsAlive(campaign::Tyrone)))
	{
		return false;
	}
	if (!TestEqual(TEXT("the cycle advanced"), Result.S.Cycle, 1))
	{
		return false;
	}

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
