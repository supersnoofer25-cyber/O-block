#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CampaignTypes.h"
#include "Campaign/Campaign.h"
#include "CampaignSubsystem.generated.h"

// The one place campaign::State lives in the running game, and the one place
// campaign::MemberID (a std::string) is translated to and from FName. Everything
// behind Campaign.h stays exactly what cpp/campaign's own tests exercise (ADR 0014) -
// this class exists only to give gameplay code and Blueprint something UE-shaped to
// call, and it is deliberately thin: it forwards to Apply and translates the answer,
// nothing more. It does not decide an encounter's outcome - that arrives from
// elsewhere as CompanionReturned (ADR 0015) and is merely handed through.
UCLASS()
class OURBLOCK_API UCampaignSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Time outside with one member (ADR 0005, ADR 0012).
	UFUNCTION(BlueprintCallable, Category = "Campaign")
	ECampaignError SpendEvening(FName Member);

	// Going out - ends the chapter (ADR 0004, ADR 0008). bCompanionReturned is the
	// encounter's result, handed in from outside per ADR 0015; this subsystem never
	// decides it, only records it, the same way campaign::Apply itself never decides it
	// (story 15).
	UFUNCTION(BlueprintCallable, Category = "Campaign")
	ECampaignError BipOut(ECampaignSeat Seat, FName Companion, bool bCompanionReturned);

	UFUNCTION(BlueprintPure, Category = "Campaign")
	bool IsRunning() const;

	UFUNCTION(BlueprintPure, Category = "Campaign")
	int32 GetChapter() const;

	UFUNCTION(BlueprintPure, Category = "Campaign")
	int32 GetCycle() const;

	UFUNCTION(BlueprintPure, Category = "Campaign")
	TArray<FName> GetAliveMembers() const;

	UFUNCTION(BlueprintPure, Category = "Campaign")
	TArray<FName> GetOutsideMembers() const;

	UFUNCTION(BlueprintPure, Category = "Campaign")
	bool IsMemberAlive(FName Member) const;

private:
	campaign::State State;
};
