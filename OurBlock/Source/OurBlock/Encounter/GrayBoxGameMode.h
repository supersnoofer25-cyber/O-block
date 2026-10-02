#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GrayBoxGameMode.generated.h"

class APlayerController;

// Sets the default pawn (AGrayBoxCharacter or ADirtBike, see bPlayerRides) so
// TestLevel's PlayerStart spawns something that can move, look, and fire - the
// engine's own DefaultPawn does none of those against ThreatActor. Also sets
// AGrayBoxHUD so there's a crosshair. Referenced from DefaultEngine.ini's
// GlobalDefaultGameMode; nothing else in the project needs a GameMode yet.
UCLASS()
class OURBLOCK_API AGrayBoxGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGrayBoxGameMode();

	// ADR 0004's per-bip seat choice, as a config toggle rather than a menu - there's
	// no prep screen to choose it from yet. false (on the back, shooting) was the
	// configuration playtested first; it was then flipped to true to build and play
	// the riding seat (Exposure mode, cover, the scripted drive). That seat played
	// "fine"/"good", so this is back to false for tuning the click-to-deny fuse
	// (spec.md open question 2) - one seat tuned at a time.
	//
	// Flip to true and rebuild to drive again. The scripted drive (-ScriptedDrive=)
	// only runs in that mode - with this false it never spawns, so rerunning the four
	// drives means flipping this first.
	//
	// Whichever seat the player doesn't take gets a placeholder occupant, not a real
	// companion - ADR 0004 itself names companion AI as load-bearing and non-trivial,
	// and improvising it inside this pass would be exactly the kind of scope creep
	// this project's whole culture exists to avoid.
	//
	// bPlayerRides == false: the passenger (player) protects a placeholder-driven
	// bike, so the driver's seat gets a bare AGrayBoxCharacter - nobody's tally is at
	// stake there, it exists only to make the seat structure visible and attachable.
	//
	// bPlayerRides == true: the companion who could be lost is on the back
	// regardless of which seat that physically is (ADR 0015), so that seat gets
	// ACompanionStandIn - the tally-holding placeholder, not a character - so
	// AThreatActor has something real to target in EThreatDenial::Exposure mode.
	UPROPERTY(EditAnywhere, Category = "Encounter")
	bool bPlayerRides = false;

protected:
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	void SpawnTheOtherSeatOccupant(APlayerController* PlayerController);

	// Exposure-mode threats can't have their Target wired up at level-edit time -
	// the companion they're aimed at doesn't exist until this GameMode spawns it on
	// the bike (SpawnTheOtherSeatOccupant), unlike the Aimed seat's companion, which
	// is a persistent level actor a Python script can reference directly. Called
	// once that companion exists; wires any Exposure threat left with no Target.
	void WireUnaimedExposureThreats(class ACompanionStandIn* Companion);
};
