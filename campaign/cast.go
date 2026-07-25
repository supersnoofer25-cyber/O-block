// Package campaign is the headless, engine-agnostic rules module for Our Block.
//
// It owns what the design commits to and nothing else: the set and who is alive,
// how time outside was spent, which seat Jamal took and who rode with him, and how
// the cycle escalates. It does not know what a gunfight is — an encounter is
// resolved elsewhere and hands the module a result, which the module makes permanent.
//
// The interface is one pure function, Apply: current state in, a player decision in,
// new state out. Nothing here renders, reads input, or depends on an engine.
//
// Vocabulary is CONTEXT.md's and is used strictly. Reasoning lives in docs/adr.
package campaign

// MemberID identifies one member of the set.
//
// Deliberately a string rather than an index: an identifier is not a number, and
// nothing on a member may be numeric (ADR 0009). Keeping IDs non-numeric also keeps
// them out of arithmetic and comparison by accident.
type MemberID string

// Capability is what a member is like on a bip.
//
// A behaviour, never a rating, and defined in both seats — a capability that existed
// in only one seat would collapse the seat choice into the companion choice, which is
// the failure ADR 0004 names. There is no field here that could order two members.
type Capability struct {
	Name   string // what he does, in a phrase
	Riding string // how it shows when he has the bike
	OnBack string // how it shows when he has the gun
}

// Member is one of the eight. Immutable for the whole campaign — only the living
// roster and familiarity change (ADR 0009).
//
// Every field is a string. That is enforced by TestStory14_MemberCarriesNoNumber and
// is the structural guard behind story 14: with nothing numeric to compare, the roster
// cannot be ranked, which is what ADR 0003 put the game's whole choice architecture on
// one screen to prevent.
type Member struct {
	ID         MemberID
	Name       string
	Capability Capability
	CostOfLoss string   // what the Block loses when he is killed
	Activity   string   // what time outside with him consists of
	Tie        MemberID // the one other member he is tied to
}

// The eight. Names and content are docs/cast.md; this is that file expressed as data.
const (
	Alex      MemberID = "alex"
	Tyrone    MemberID = "tyrone"
	Anthony   MemberID = "anthony"
	James     MemberID = "james"
	Daquavion MemberID = "daquavion"
	Marcus    MemberID = "marcus"
	Omar      MemberID = "omar"
	Joshua    MemberID = "joshua"
)

// TheSet is the roster at campaign start, in a fixed order.
//
// A slice rather than a map, so iteration order is stable and story 28's determinism
// cannot be broken by Go's randomised map ordering.
//
// There is deliberately no exported way to append to this or to build a different set:
// story 13 forecloses recruitment and replacement, and the cheapest way to guarantee
// that is to give the package no function that could do it.
var TheSet = []Member{
	{
		ID:   Alex,
		Name: "Alex",
		Capability: Capability{
			Name:   "Doesn't hesitate",
			Riding: "Commits to the approach, closer than you'd have chosen",
			OnBack: "Fires first, before you've decided this is happening",
		},
		CostOfLoss: "His little sister stops coming outside. She is still on the Block; you just don't see her any more.",
		Activity:   "Around, with his sister there",
		Tie:        Daquavion,
	},
	{
		ID:   Tyrone,
		Name: "Tyrone",
		Capability: Capability{
			Name:   "Won't shut up",
			Riding: "Calling what's ahead before you reach it",
			OnBack: "Telling you what's behind you",
		},
		CostOfLoss: "He sells food. After Tyrone the Block doesn't eat the way it did.",
		Activity:   "At his spot while he sells",
		Tie:        James,
	},
	{
		ID:   Anthony,
		Name: "Anthony",
		Capability: Capability{
			Name:   "The one who says no",
			Riding: "Takes the careful exit",
			OnBack: "Holds until he's sure",
		},
		CostOfLoss: "He has a job. After Anthony nobody on the Block is working — the thread to an ordinary life goes with him.",
		Activity:   "Caught before or after work",
		Tie:        Marcus,
	},
	{
		ID:   James,
		Name: "James",
		Capability: Capability{
			Name:   "Doesn't leave early",
			Riding: "Won't pull out while anyone is still exposed",
			OnBack: "Keeps firing until it's actually done",
		},
		CostOfLoss: "He fixes the church roof. After James it doesn't get fixed, and the Block uses less of where it gathers.",
		Activity:   "Up on the church roof with him",
		Tie:        Tyrone,
	},
	{
		ID:   Daquavion,
		Name: "Daquavion",
		Capability: Capability{
			Name:   "Moves first",
			Riding: "Already moving when it goes wrong",
			OnBack: "Already firing when it goes wrong",
		},
		CostOfLoss: "He has a newborn. After Daquavion his girl takes the baby off the Block.",
		Activity:   "At his place, around the baby",
		Tie:        Alex,
	},
	{
		ID:   Marcus,
		Name: "Marcus",
		Capability: Capability{
			Name:   "Knows the way out",
			Riding: "Takes exits you wouldn't have found",
			OnBack: "Telling you where to go before you've asked",
		},
		CostOfLoss: "He works on cars. After Marcus they stop running — not his, everyone's.",
		Activity:   "Under a car",
		Tie:        Anthony,
	},
	{
		ID:   Omar,
		Name: "Omar",
		Capability: Capability{
			Name:   "Acts like nothing is happening",
			Riding: "Never tenses up",
			OnBack: "The calmest gun in the set, because he hasn't quite registered that it's real",
		},
		CostOfLoss: "He has a basketball hoop. After Omar nobody plays. It stands there and nobody uses it.",
		Activity:   "At the hoop",
		Tie:        Joshua,
	},
	{
		ID:   Joshua,
		Name: "Joshua",
		Capability: Capability{
			Name:   "Doesn't flinch",
			Riding: "Goes where you point without asking",
			OnBack: "Doesn't look away",
		},
		CostOfLoss: "He lives with Jamal. After Joshua, the house Jamal comes back to is empty.",
		Activity:   "At home, because you live there",
		Tie:        Omar,
	},
}

// MemberByID returns the immutable definition of a member.
func MemberByID(id MemberID) (Member, bool) {
	for _, m := range TheSet {
		if m.ID == id {
			return m, true
		}
	}
	return Member{}, false
}
