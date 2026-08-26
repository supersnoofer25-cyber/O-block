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
	// no prep screen to choose it from yet. false (on the back, shooting) is the
	// configuration already playtested; true (riding) exercises the half of ADR 0015
	// that's never been tested at all, and needs its own pass once this is stable.
	//
	// Whichever seat the player doesn't take gets a placeholder occupant, not a real
	// companion - ADR 0004 itself names companion AI as load-bearing and non-trivial,
	// and improvising it inside this pass would be exactly the kind of scope creep
	// this project's whole culture exists to avoid. The placeholder rides along and,
	// on the passenger side, can still be shot at by nothing (it's not a ThreatActor)
	// - it exists only to make the seat structure visible and attachable, not to act.
	UPROPERTY(EditAnywhere, Category = "Encounter")
	bool bPlayerRides = false;

protected:
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	void SpawnTheOtherSeatOccupant(APlayerController* PlayerController);
};
