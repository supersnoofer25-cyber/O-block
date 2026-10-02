#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThreatActor.generated.h"

class UDangerTallyComponent;
class UStaticMeshComponent;

// Which of ADR 0015's two seat-specific meanings of "denying a threat" this actor uses.
// The tally, the threshold, and what happens on Fire() are identical either way - only
// how denial gets decided differs, mirroring the split ADR 0004 made between the seats.
UENUM(BlueprintType)
enum class EThreatDenial : uint8
{
	// Companion on the back, player shooting: denial is a discrete event a click
	// produces. Deny() is called from outside (AGrayBoxCharacter's fire trace); the
	// fuse runs unconditionally from BeginPlay, exactly as it always has.
	Aimed,

	// Player riding, companion on the back returning fire: ADR 0015 fixes the
	// companion's aim as always competent, so there is nothing to click - the only
	// lever left to the player is route and timing. TickExposure accumulates time
	// only while the threat has a clear, in-range line to its Target and fires once
	// that reaches TimeToFire; losing the sightline resets the clock rather than
	// pausing it, so ducking out of view for even a moment buys a clean reset, not
	// partial credit toward the next time this threat gets an angle.
	Exposure,
};

// A threat, gray-boxed: a sphere that either gets denied or fires. Nothing here is the
// actual combat ADR 0015 describes - it is a stand-in for "the gun the player put down
// before it came up," cheap enough to playtest with before any of the real animation,
// AI, or shooting exists. What matters for validating the mechanic is the shape of the
// interaction, not the fidelity: a window of time, something the player can act on, and
// a consequence if they don't.
//
// Deny() stands in for the Aimed seat's meaning of denying a threat - killing or
// suppressing it before it fires. Exposure mode never calls Deny(); see EThreatDenial.
UCLASS()
class OURBLOCK_API AThreatActor : public AActor
{
	GENERATED_BODY()

public:
	AThreatActor();

	UPROPERTY(EditAnywhere, Category = "Encounter")
	EThreatDenial DenialMode = EThreatDenial::Aimed;

	// How long this threat has before it fires if nobody denies it (Aimed), or how
	// much sustained, unbroken exposure it needs before it fires (Exposure).
	UPROPERTY(EditAnywhere, Category = "Encounter")
	float TimeToFire = 3.0f;

	// Exposure mode only: beyond this range the threat can't get an angle at all,
	// regardless of sightline. Meaningless in Aimed mode.
	UPROPERTY(EditAnywhere, Category = "Encounter")
	float ExposureRange = 1500.0f;

	// Exposure mode only: how much unbroken sightline time this threat has accrued
	// so far this bip. Never shown to the player (ADR 0015) - VisibleAnywhere here is
	// for debugging the gray-box in the editor, not player-facing presentation.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Encounter")
	float ExposureTime = 0.0f;

	// How much the tally moves if this threat connects. 1 by default; exists as a
	// property rather than a hardcoded constant so a harder threat can weigh more
	// without needing a second mechanic to say so.
	UPROPERTY(EditAnywhere, Category = "Encounter")
	int32 DangerOnFire = 1;

	// The companion this threat is aimed at. A threat with no target simply never adds
	// danger to anything, which is deliberate rather than an error case - it lets a
	// threat exist in the world before the encounter wires it to anyone.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter")
	TObjectPtr<UDangerTallyComponent> Target;

	// Deny this threat. Idempotent, and has no effect once it has already fired -
	// there is no undoing a shot that already landed. Aimed mode only; nothing calls
	// this in Exposure mode (see EThreatDenial).
	UFUNCTION(BlueprintCallable, Category = "Encounter")
	void Deny();

	// Exposed for the test and for anything that wants to force resolution without
	// waiting for the timer (e.g., an encounter ending early).
	UFUNCTION(BlueprintCallable, Category = "Encounter")
	void Fire();

	// The pure logic half of Exposure mode: given whether this threat currently has
	// the angle, advance or reset the exposure clock and fire if it's run out. Kept
	// separate from Tick() (which decides bHasSightline via a world trace) so the
	// clock/threshold behaviour is unit-testable the same way Deny()/Fire() are,
	// without needing an actual World to trace against.
	UFUNCTION(BlueprintCallable, Category = "Encounter")
	void TickExposure(float DeltaSeconds, bool bHasSightline);

	UFUNCTION(BlueprintPure, Category = "Encounter")
	bool IsDenied() const { return bDenied; }

	UFUNCTION(BlueprintPure, Category = "Encounter")
	bool HasFired() const { return bFired; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, Category = "Encounter")
	TObjectPtr<UStaticMeshComponent> Mesh;

private:
	// Exposure mode's world query: is there a clear, in-range line from this threat to
	// Target's owner right now? Deliberately separate from TickExposure() so the world
	// trace (untestable without a World) never touches the clock/threshold logic
	// (which is).
	bool HasSightlineToTarget() const;

	FTimerHandle FireTimer;
	bool bDenied = false;
	bool bFired = false;
};
