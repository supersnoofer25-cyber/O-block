#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DangerTallyComponent.generated.h"

// The hidden per-bip tally ADR 0015 describes: danger the player did not deny in time,
// resolved by ordinary skill-driven combat and never a roll. This component only ever
// counts what it's told - it has no opinion about what counts as a threat, whether a
// shot connects, or how a threat behaves. That's AThreatActor's job and, eventually,
// the encounter's; keeping this component to bookkeeping is what lets the same tally
// serve both seats (ADR 0004) and every member (ADR 0009) without knowing which.
//
// Threshold is fixed and identical for every companion. A threshold that varied by
// member would be exactly the stat ADR 0009 forecloses - the same failure as a
// competence rating, arriving through a back door. The number itself is a placeholder;
// what this component exists to validate is the mechanic, not this value, and only
// playing it can answer what the value should be (spec.md open question 2).
UCLASS(ClassGroup = (Encounter), meta = (BlueprintSpawnableComponent))
class OURBLOCK_API UDangerTallyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter")
	int32 Threshold = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Encounter")
	int32 Tally = 0;

	// Called when a threat was not denied in time. Never called for a threat that was
	// killed or suppressed before it could fire - that distinction is made by whatever
	// resolved the threat, not by this component.
	UFUNCTION(BlueprintCallable, Category = "Encounter")
	void AddDanger(int32 Amount = 1);

	// Whether tonight's tally crossed the line. A single fixed threshold, resolved from
	// the tally - never a probability, never anything shown to the player (ADR 0015).
	UFUNCTION(BlueprintPure, Category = "Encounter")
	bool DidCompanionSurvive() const { return Tally < Threshold; }

	UFUNCTION(BlueprintCallable, Category = "Encounter")
	void Reset() { Tally = 0; }
};
