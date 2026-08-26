#include "CampaignSubsystem.h"
#include "OurBlock.h"

namespace
{
	// campaign::MemberID is a std::string; FName is what Blueprint and the rest of the
	// engine use for identifiers. This is the only place that conversion happens -
	// nothing behind Campaign.h ever sees an FName, and nothing outside this file ever
	// sees a std::string.
	std::string ToStdString(FName Name)
	{
		return TCHAR_TO_UTF8(*Name.ToString());
	}

	FName ToFName(const std::string& Value)
	{
		return FName(UTF8_TO_TCHAR(Value.c_str()));
	}

	campaign::Seat ToCampaignSeat(ECampaignSeat Seat)
	{
		switch (Seat)
		{
		case ECampaignSeat::Riding: return campaign::Seat::Riding;
		case ECampaignSeat::OnBack: return campaign::Seat::OnBack;
		}
		return campaign::Seat::None;
	}

	ECampaignError ToCampaignError(campaign::Error Err)
	{
		switch (Err)
		{
		case campaign::Error::None: return ECampaignError::None;
		case campaign::Error::Ended: return ECampaignError::Ended;
		case campaign::Error::NotAMember: return ECampaignError::NotAMember;
		case campaign::Error::Dead: return ECampaignError::Dead;
		case campaign::Error::GoneInside: return ECampaignError::GoneInside;
		case campaign::Error::Seat: return ECampaignError::Seat;
		}
		return ECampaignError::None;
	}
}

void UCampaignSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	State = campaign::New();
	UE_LOG(LogOurBlock, Log, TEXT("UCampaignSubsystem initialized: %d alive, chapter %d"),
		static_cast<int32>(State.Alive.size()), State.Chapter);
}

ECampaignError UCampaignSubsystem::SpendEvening(FName Member)
{
	campaign::SpendEvening Decision;
	Decision.With = ToStdString(Member);

	campaign::Result Result = campaign::Apply(State, Decision);
	if (Result.Ok())
	{
		State = Result.S;
	}
	return ToCampaignError(Result.Err);
}

ECampaignError UCampaignSubsystem::BipOut(ECampaignSeat Seat, FName Companion, bool bCompanionReturned)
{
	campaign::BipOut Decision;
	Decision.Seat = ToCampaignSeat(Seat);
	Decision.Companion = ToStdString(Companion);
	Decision.CompanionReturned = bCompanionReturned;

	campaign::Result Result = campaign::Apply(State, Decision);
	if (Result.Ok())
	{
		State = Result.S;
	}
	return ToCampaignError(Result.Err);
}

bool UCampaignSubsystem::IsRunning() const
{
	return State.Running();
}

int32 UCampaignSubsystem::GetChapter() const
{
	return State.Chapter;
}

int32 UCampaignSubsystem::GetCycle() const
{
	return State.Cycle;
}

TArray<FName> UCampaignSubsystem::GetAliveMembers() const
{
	TArray<FName> Out;
	Out.Reserve(State.Alive.size());
	for (const campaign::MemberID& Id : State.Alive)
	{
		Out.Add(ToFName(Id));
	}
	return Out;
}

TArray<FName> UCampaignSubsystem::GetOutsideMembers() const
{
	TArray<FName> Out;
	Out.Reserve(State.Outside.size());
	for (const campaign::MemberID& Id : State.Outside)
	{
		Out.Add(ToFName(Id));
	}
	return Out;
}

bool UCampaignSubsystem::IsMemberAlive(FName Member) const
{
	return State.IsAlive(ToStdString(Member));
}
