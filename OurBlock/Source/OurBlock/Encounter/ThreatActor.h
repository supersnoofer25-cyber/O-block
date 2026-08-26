#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThreatActor.generated.h"

class UDangerTallyComponent;
class UStaticMeshComponent;

// A threat, gray-boxed: a sphere that either gets denied or fires. Nothing here is the
// actual combat ADR 0015 describes - it is a stand-in for "the gun the player put down
// before it came up," cheap enough to playtest with before any of the real animation,
// AI, or shooting exists. What matters for validating the mechanic is the shape of the
// interaction, not the fidelity: a window of time, something the player can act on, and
// a consequence if they don't.
//
// Deny() stands in for both of ADR 0015's seat-specific meanings of denying a threat -
// killing/suppressing it from the back seat, or never giving it the angle while riding.
// Which one it represents is a decision for whatever calls Deny(), not for this actor.
UCLASS()
class OURBLOCK_API AThreatActor : public AActor
{
	GENERATED_BODY()

public:
	AThreatActor();

	// How long this threat has before it fires if nobody denies it.
	UPROPERTY(EditAnywhere, Category = "Encounter")
	float TimeToFire = 3.0f;

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
	// there is no undoing a shot that already landed.
	UFUNCTION(BlueprintCallable, Category = "Encounter")
	void Deny();

	// Exposed for the test and for anything that wants to force resolution without
	// waiting for the timer (e.g., an encounter ending early).
	UFUNCTION(BlueprintCallable, Category = "Encounter")
	void Fire();

	UFUNCTION(BlueprintPure, Category = "Encounter")
	bool IsDenied() const { return bDenied; }

	UFUNCTION(BlueprintPure, Category = "Encounter")
	bool HasFired() const { return bFired; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Encounter")
	TObjectPtr<UStaticMeshComponent> Mesh;

private:
	FTimerHandle FireTimer;
	bool bDenied = false;
	bool bFired = false;
};
