#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CompanionStandIn.generated.h"

class UDangerTallyComponent;
class UStaticMeshComponent;
class ADirtBike;

// A placeholder for the companion this gray-box protects - a visible cube and nothing
// else, so the player has something to stand near and something ThreatActors can be
// aimed at. Not a companion in any of the senses ADR 0006/0009 mean: no behaviour, no
// capability, no cost of loss. It exists only to hold the DangerTallyComponent
// somewhere visible in the scene.
//
// AttachToBikeSeat makes this actor double as the "player rides, companion shoots"
// seat's occupant (ADR 0015): the companion who could be lost sits on the back
// regardless of which physical seat that is, so this is the right actor for that role
// too, not AGrayBoxCharacter - a full player-character placeholder never had a tally
// to hold and was never going to act on its own (companion AI is explicitly deferred,
// see GrayBoxGameMode). GrayBoxGameMode spawns this here instead when bPlayerRides.
UCLASS()
class OURBLOCK_API ACompanionStandIn : public AActor
{
	GENERATED_BODY()

public:
	ACompanionStandIn();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Encounter")
	TObjectPtr<UDangerTallyComponent> Tally;

	// Attaches to a bike's passenger seat so this stand-in - and the tally it holds -
	// moves with the bike. Mirrors AGrayBoxCharacter::AttachToBikeSeat's seat-height
	// correction: the seat marks the bike's top surface, not this actor's own origin,
	// so the mesh's own half-height (a documented constant, not a magic number - the
	// default engine cube this actor uses is 100 units per side, unscaled) is added on
	// top the same way AttachToBikeSeat there adds the capsule's half-height.
	UFUNCTION(BlueprintCallable, Category = "Encounter")
	void AttachToBikeSeat(ADirtBike* Bike);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Encounter")
	TObjectPtr<UStaticMeshComponent> Mesh;
};
