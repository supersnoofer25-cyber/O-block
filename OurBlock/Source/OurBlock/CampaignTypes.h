#pragma once

#include "CoreMinimal.h"
#include "CampaignTypes.generated.h"

// Mirrors campaign::Seat for Blueprint. campaign::Seat::None has no counterpart here on
// purpose - it exists in the pure module only as BipOut's invalid default, never a seat
// anyone actually chooses. ADR 0004 makes the seat exactly Riding or on-the-back,
// picked fresh every bip; a third value here would misrepresent that as a choice.
UENUM(BlueprintType)
enum class ECampaignSeat : uint8
{
	Riding,
	OnBack,
};

// Mirrors campaign::Error - the only way Apply reports a decision that couldn't be
// taken. Losing a companion is deliberately not one of these values (story 24, ADR
// 0002): a death is not a failure, so it can't arrive on the same channel as one.
UENUM(BlueprintType)
enum class ECampaignError : uint8
{
	None,
	Ended,
	NotAMember,
	Dead,
	GoneInside,
	Seat,
};
