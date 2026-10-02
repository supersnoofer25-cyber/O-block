// Proves the gray-box danger-tally mechanic (ADR 0015) behaves correctly before any
// human plays it: a threat denied in time adds nothing, a threat left alone adds
// exactly what it says it will, and the tally - not a roll - is what decides the
// outcome. This is the logic layer; whether the mechanic *feels* right, which is the
// actual point of gray-boxing, can only be judged by playing TestLevel, not by a test.
//
// Run headlessly the same way as OurBlock.Campaign's suite:
//   UnrealEditor-Cmd.exe OurBlock.uproject -ExecCmds="Automation RunTests OurBlock.Encounter; Quit" -unattended -nopause -nullrhi
#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Encounter/DangerTallyComponent.h"
#include "Encounter/ThreatActor.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FThreatDeniedAddsNothingTest, "OurBlock.Encounter.ThreatDeniedAddsNothing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FThreatDeniedAddsNothingTest::RunTest(const FString& Parameters)
{
	UDangerTallyComponent* Tally = NewObject<UDangerTallyComponent>();
	AThreatActor* Threat = NewObject<AThreatActor>();
	Threat->Target = Tally;

	Threat->Deny();
	Threat->Fire(); // must be a no-op: denied already, and Fire() checks bDenied first

	if (!TestTrue(TEXT("threat reports denied"), Threat->IsDenied())) return false;
	if (!TestFalse(TEXT("threat does not also report fired"), Threat->HasFired())) return false;
	if (!TestEqual(TEXT("tally is untouched"), Tally->Tally, 0)) return false;
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FThreatFiredAddsItsDangerTest, "OurBlock.Encounter.ThreatFiredAddsItsDanger",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FThreatFiredAddsItsDangerTest::RunTest(const FString& Parameters)
{
	UDangerTallyComponent* Tally = NewObject<UDangerTallyComponent>();
	AThreatActor* Threat = NewObject<AThreatActor>();
	Threat->Target = Tally;
	Threat->DangerOnFire = 2;

	Threat->Fire();

	if (!TestTrue(TEXT("threat reports fired"), Threat->HasFired())) return false;
	if (!TestFalse(TEXT("threat does not also report denied"), Threat->IsDenied())) return false;
	if (!TestEqual(TEXT("tally moved by exactly this threat's weight"), Tally->Tally, 2)) return false;
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFiringIsFinalTest, "OurBlock.Encounter.FiringIsFinal",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FFiringIsFinalTest::RunTest(const FString& Parameters)
{
	// There is no undoing a shot that already landed - denying a threat after it has
	// fired must not retroactively erase the danger it already added.
	UDangerTallyComponent* Tally = NewObject<UDangerTallyComponent>();
	AThreatActor* Threat = NewObject<AThreatActor>();
	Threat->Target = Tally;

	Threat->Fire();
	Threat->Deny();

	if (!TestTrue(TEXT("threat still reports fired"), Threat->HasFired())) return false;
	if (!TestFalse(TEXT("a late deny does not become true"), Threat->IsDenied())) return false;
	if (!TestEqual(TEXT("the danger already added is still there"), Tally->Tally, 1)) return false;
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FExposureBreakingSightlineResetsTheClockTest, "OurBlock.Encounter.ExposureBreakingSightlineResetsTheClock",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FExposureBreakingSightlineResetsTheClockTest::RunTest(const FString& Parameters)
{
	// The riding seat's whole mechanic (ADR 0015): a threat that nearly had enough
	// sustained sightline to fire, then loses it, must not carry that progress into
	// the next time it re-acquires an angle - a clean break is a clean reset, not a
	// pause, or "never giving it the angle" would stop meaning what it says.
	UDangerTallyComponent* Tally = NewObject<UDangerTallyComponent>();
	AThreatActor* Threat = NewObject<AThreatActor>();
	Threat->DenialMode = EThreatDenial::Exposure;
	Threat->TimeToFire = 3.0f;
	Threat->Target = Tally;

	Threat->TickExposure(2.9f, true); // almost there
	Threat->TickExposure(1.0f, false); // breaks - resets, not pauses
	Threat->TickExposure(2.9f, true); // alone, still short of 3.0f again

	if (!TestFalse(TEXT("threat has not fired - no single unbroken window reached TimeToFire"), Threat->HasFired())) return false;
	if (!TestEqual(TEXT("tally untouched"), Tally->Tally, 0)) return false;
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FExposureSustainedLongEnoughFiresTest, "OurBlock.Encounter.ExposureSustainedLongEnoughFires",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FExposureSustainedLongEnoughFiresTest::RunTest(const FString& Parameters)
{
	UDangerTallyComponent* Tally = NewObject<UDangerTallyComponent>();
	AThreatActor* Threat = NewObject<AThreatActor>();
	Threat->DenialMode = EThreatDenial::Exposure;
	Threat->TimeToFire = 3.0f;
	Threat->Target = Tally;

	Threat->TickExposure(1.5f, true);
	if (!TestFalse(TEXT("not yet - short of TimeToFire"), Threat->HasFired())) return false;

	Threat->TickExposure(1.5f, true); // 3.0f total, unbroken
	if (!TestTrue(TEXT("fires once sustained sightline reaches TimeToFire"), Threat->HasFired())) return false;
	if (!TestEqual(TEXT("tally moved by this threat's weight"), Tally->Tally, 1)) return false;
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTallyOutcomeIsAThresholdNotARollTest, "OurBlock.Encounter.TallyOutcomeIsAThresholdNotARoll",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FTallyOutcomeIsAThresholdNotARollTest::RunTest(const FString& Parameters)
{
	UDangerTallyComponent* Tally = NewObject<UDangerTallyComponent>();
	Tally->Threshold = 2;

	// Two threats denied, one gets through: below threshold, companion comes home.
	AThreatActor* Denied1 = NewObject<AThreatActor>();
	Denied1->Target = Tally;
	Denied1->Deny();

	AThreatActor* Denied2 = NewObject<AThreatActor>();
	Denied2->Target = Tally;
	Denied2->Deny();

	AThreatActor* GotThrough = NewObject<AThreatActor>();
	GotThrough->Target = Tally;
	GotThrough->Fire();

	if (!TestEqual(TEXT("tally reflects only the one threat that connected"), Tally->Tally, 1)) return false;
	if (!TestTrue(TEXT("below threshold, the companion comes home"), Tally->DidCompanionSurvive())) return false;

	// A second threat lands: at threshold, companion does not come home. Deterministic
	// from the tally alone - the same inputs must always produce the same outcome,
	// which is what "never a roll" means in practice.
	AThreatActor* TipsItOver = NewObject<AThreatActor>();
	TipsItOver->Target = Tally;
	TipsItOver->Fire();

	if (!TestEqual(TEXT("tally is now exactly at the threshold"), Tally->Tally, 2)) return false;
	if (!TestFalse(TEXT("at threshold, the companion does not come home"), Tally->DidCompanionSurvive())) return false;
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
