#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScriptedDrive.generated.h"

class ADirtBike;
class AThreatActor;
class UDangerTallyComponent;

// A dev harness, not part of the game: drives the bike along a fixed route with no
// human at the keys, logs what every Exposure threat saw, then quits. It answers
// "does Exposure mode actually work in a real level" - range cutoff, sightline trace,
// target wiring, reset-on-break - so a human's play time goes on how it *feels*, which
// no script can judge (ADR 0015; spec.md open question 2).
//
// Spawned by AGrayBoxGameMode only when the game is launched with -ScriptedDrive=<mode>,
// and only in the riding seat. Runs headless and fast:
//
//   UnrealEditor-Cmd.exe OurBlock.uproject /Game/Maps/TestLevel -game -nullrhi
//       -unattended -nosplash -benchmark -fps=60 -ScriptedDrive=camp
//
// -benchmark -fps=60 fixes the timestep, so a run's timings don't depend on how fast
// the machine happens to be. Modes, each a question about the three Exposure threats
// populate_exposure_test.py places:
//   park - never move. All three are out of range of the start; none may fire.
// Both driving modes go up the X=300 lane: X=0 runs straight into the click-to-deny
// cluster populate_test_encounter.py left around the origin (its companion cube at
// (0,0) stopped the first version of this dead at Y=-160).
//   camp - drive into the middle of them and stop. All in range, nothing breaks the
//          sightline (no cover yet); all three must fire, roughly TimeToFire after
//          each first got the angle.
//   pass - drive straight through at full speed and out the far side. No threat keeps
//          the angle for TimeToFire; none may fire.
//
// The log line to read is the final "ScriptedDrive[...] RESULT", plus one status line
// per second showing each threat's distance and exposure clock.
UCLASS()
class OURBLOCK_API AScriptedDrive : public AActor
{
	GENERATED_BODY()

public:
	AScriptedDrive();

	// Called by AGrayBoxGameMode right after spawning. Returns false (and logs why) for
	// an unknown mode, in which case the caller should destroy this.
	bool Start(const FString& InMode, ADirtBike* InBike, UDangerTallyComponent* InTally);

protected:
	virtual void Tick(float DeltaSeconds) override;

private:
	void Steer();
	void LogStatus() const;
	void Finish();

	FString Mode;
	TWeakObjectPtr<ADirtBike> Bike;
	TWeakObjectPtr<UDangerTallyComponent> Tally;

	// Gathered once at Start. Weak because a threat destroys itself when it fires.
	TArray<TWeakObjectPtr<AThreatActor>> ExposureThreats;
	TArray<FString> ExposureThreatNames;

	// Waypoints in order; the bike drives through each and brakes to a stop at the last.
	// Empty means park.
	TArray<FVector2D> Route;
	int32 RouteIndex = 0;
	bool bArrived = false;
	bool bReportedBlocked = false;

	float Elapsed = 0.f;
	float NextStatusAt = 0.f;
	bool bFinished = false;

	// Long enough for a camped threat to reach TimeToFire (5s) after the drive in, with
	// margin; short enough that a run takes seconds of real time headless.
	static constexpr float Duration = 15.f;
};
