#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CompanionStandIn.generated.h"

class UDangerTallyComponent;
class UStaticMeshComponent;

// A placeholder for the companion this gray-box protects - a visible cube and nothing
// else, so the player has something to stand near and something ThreatActors can be
// aimed at. Not a companion in any of the senses ADR 0006/0009 mean: no behaviour, no
// capability, no cost of loss. It exists only to hold the DangerTallyComponent
// somewhere visible in the scene.
UCLASS()
class OURBLOCK_API ACompanionStandIn : public AActor
{
	GENERATED_BODY()

public:
	ACompanionStandIn();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Encounter")
	TObjectPtr<UDangerTallyComponent> Tally;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Encounter")
	TObjectPtr<UStaticMeshComponent> Mesh;
};
