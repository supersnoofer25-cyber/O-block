package campaign

import "sort"

// Tuning values. Named and gathered because they are tuning rather than decision —
// spec.md open question 2 is explicit that whether the encounters as built hit the
// target can only be answered by playing them.
const (
	// Chapters is the fixed length of a campaign (ADR 0008). The ending is tied to
	// this rather than to the roster: how many are alive when it arrives is the
	// variable.
	//
	// Was sixteen, which ADR 0008 offered explicitly as a starting point and as
	// tuning rather than decision. Playtesting the prototype found the campaign going
	// flat around the tenth chapter with the ambient content and flares in place — so
	// with the confound that would have explained it away already removed. That is the
	// case ADR 0008 said to answer by cutting chapters rather than by adding an act
	// structure, because the flatness is carrying an argument and the length is not.
	//
	// Twelve is the shortest cut that clears where it sagged while staying above the
	// floor: the count must exceed the roster or an empty set is unreachable and ADR
	// 0007's central case becomes fiction.
	Chapters = 12

	// IntensityCeiling is where escalation stops climbing (ADR 0013). It rises over
	// roughly the first third and the remaining chapters sit at the top; the last
	// chapter is no more intense than the middle one.
	//
	// Cut with the campaign, because "roughly the first third" is the part ADR 0013
	// decided and the number is what follows from it. A third of twelve is four.
	IntensityCeiling = 4

	// DepartAlongside is how many other members go inside each time Jamal spends an
	// evening with someone. He was with Marcus, so he was not there when Omar went
	// upstairs (ADR 0012). Nothing is deducted from the player; people simply leave.
	DepartAlongside = 3

	// EveningsToClose is how many evenings with one member it takes to know him well.
	//
	// This is what makes story 4 hold, and it holds arithmetically rather than by
	// hope: a chapter yields at most ceil(8/(1+DepartAlongside)) = 2 evenings, so a
	// campaign yields at most 24, while knowing all eight well would take 8 * 5 = 40.
	// Reaching high familiarity with the whole set is impossible before anyone has
	// even died. Cutting the campaign to twelve widened that margin rather than
	// threatening it, so the story holds by more than it did.
	EveningsToClose = 5
)

// Level is how well Jamal knows someone, in words rather than as a number.
//
// The underlying count is never surfaced: ADR 0006 forecloses familiarity as a meter
// or tier the player can read, and ADR 0012 forecloses anything countable outside.
// This exists so the rules can talk about it, not so a UI can draw it.
type Level string

const (
	Stranger   Level = "stranger"
	Acquainted Level = "acquainted"
	Known      Level = "known"
	Close      Level = "close"
)

// Seat is where Jamal sits on a given bip (ADR 0004). Chosen fresh every time.
type Seat string

const (
	// Riding means owning the approach and the escape, trusting someone else's aim.
	Riding Seat = "riding"
	// OnBack means owning the violence, trusting someone else to get you out.
	OnBack Seat = "on-the-back"
)

// EndReason is why a campaign stopped.
//
// Neither value is a failure, and there is deliberately no third value that would be.
// ADR 0002 forbids presenting a set member's death as a losing outcome, and the
// cheapest way to guarantee that here is to give the type nowhere to put one.
type EndReason string

const (
	// NotEnded is the zero value, so a fresh campaign is running by default.
	NotEnded EndReason = ""
	// Finished is the campaign reaching its last chapter (ADR 0007).
	Finished EndReason = "finished"
	// SetEmpty is the campaign reaching the same ending early, because the loop
	// cannot run with nobody to fill the other seat. Not a failure (ADR 0007).
	SetEmpty EndReason = "set-empty"
)

// Familiarity is the count of evenings Jamal has spent with one member.
type familiarity struct {
	Member   MemberID `json:"member"`
	Evenings int      `json:"evenings"`
}

// lastBip is what happened the last time Jamal went out.
//
// Tonight's reason to go comes from this and from nothing else (ADR 0011), which is
// why the module has to remember it: with every external flare generator foreclosed —
// no schedule, no escalation manager, no quest-giver — the previous bip is the only
// thing left that could cause the next one. Its zero value means the campaign has not
// had a bip yet, and chapter one needs no special case because the beef is inherited.
type lastBip struct {
	Happened bool     `json:"happened"`
	Lost     bool     `json:"lost"`
	Who      MemberID `json:"who"`
}

// State is the whole campaign. Every field is exported and JSON-tagged so the state
// round-trips (story 26), and every collection is a slice in a stable order so that
// identical decisions reproduce identical state (story 28) — Go randomises map
// iteration, which would quietly break that.
type State struct {
	Chapter int `json:"chapter"` // 1-based; Chapters is the last

	// Alive is the living roster, in TheSet's order. It only ever shortens
	// (stories 22, 23).
	Alive []MemberID `json:"alive"`

	// Outside is who is still out this evening. Empty means the only thing left to
	// do is go (ADR 0012).
	Outside []MemberID `json:"outside"`

	// Familiarity is per member and never decreases (stories 3, 5).
	Familiarity []familiarity `json:"familiarity"`

	// Cycle counts completed bips. It advances on every bip regardless of outcome
	// (stories 16, 17) and nothing reduces it (story 18).
	Cycle int `json:"cycle"`

	// LastBip is what the next flare is made of (ADR 0011).
	LastBip lastBip `json:"last_bip"`

	End EndReason `json:"end"`
}

// New starts a campaign: chapter one, all eight alive and outside.
//
// Joshua begins at Close. You cannot not know your own brother — he is Jamal's
// younger brother and lives with him (docs/cast.md). This is not the special case the
// cast contract forbids: the contract governs which properties a member carries, and
// every member carries the same ones. A starting value is not a property.
func New() State {
	s := State{Chapter: 1}
	for _, m := range TheSet {
		s.Alive = append(s.Alive, m.ID)
		s.Familiarity = append(s.Familiarity, familiarity{Member: m.ID})
	}
	s.setEvenings(Joshua, EveningsToClose)
	s.Outside = append([]MemberID(nil), s.Alive...)
	return s
}

// Running reports whether the campaign is still going.
func (s State) Running() bool { return s.End == NotEnded }

// IsAlive reports whether a member is still alive.
func (s State) IsAlive(id MemberID) bool { return contains(s.Alive, id) }

// IsOutside reports whether a member is still out this evening.
func (s State) IsOutside(id MemberID) bool { return contains(s.Outside, id) }

// Evenings is how many evenings Jamal has spent with a member.
func (s State) Evenings(id MemberID) int {
	for _, f := range s.Familiarity {
		if f.Member == id {
			return f.Evenings
		}
	}
	return 0
}

// Knows is how well Jamal knows a member, in words.
func (s State) Knows(id MemberID) Level {
	switch n := s.Evenings(id); {
	case n >= EveningsToClose:
		return Close
	case n >= EveningsToClose/2:
		return Known
	case n > 0:
		return Acquainted
	default:
		return Stranger
	}
}

// Intensity is how bad things are now, which is not the same as how far the cycle has
// run. It climbs with the cycle and then stops (ADR 0013).
//
// The two are separate on purpose. Stories 16 and 17 require every bip to escalate the
// cycle regardless of outcome, and ADR 0013 requires the intensity the player meets to
// plateau. A single counter cannot satisfy both; Cycle always advances, Intensity caps.
func (s State) Intensity() int {
	if s.Cycle > IntensityCeiling {
		return IntensityCeiling
	}
	return s.Cycle
}

// Companions are the members who could fill the other seat right now: the living, in
// a stable order (story 12).
func (s State) Companions() []MemberID {
	return append([]MemberID(nil), s.Alive...)
}

// setEvenings is the only writer of familiarity, and it never lowers a value
// (story 5).
func (s *State) setEvenings(id MemberID, n int) {
	for i := range s.Familiarity {
		if s.Familiarity[i].Member == id && n > s.Familiarity[i].Evenings {
			s.Familiarity[i].Evenings = n
			return
		}
	}
}

func contains(ids []MemberID, id MemberID) bool {
	for _, x := range ids {
		if x == id {
			return true
		}
	}
	return false
}

// remove returns ids without id, preserving order.
func remove(ids []MemberID, id MemberID) []MemberID {
	out := make([]MemberID, 0, len(ids))
	for _, x := range ids {
		if x != id {
			out = append(out, x)
		}
	}
	return out
}

// inSetOrder sorts ids into TheSet's declared order, so that no operation can leave a
// collection in an order that depends on how it was built (story 28).
func inSetOrder(ids []MemberID) []MemberID {
	rank := map[MemberID]int{}
	for i, m := range TheSet {
		rank[m.ID] = i
	}
	out := append([]MemberID(nil), ids...)
	sort.Slice(out, func(i, j int) bool { return rank[out[i]] < rank[out[j]] })
	return out
}

// clone deep-copies a state so Apply can stay a pure function.
func (s State) clone() State {
	out := s
	out.Alive = append([]MemberID(nil), s.Alive...)
	out.Outside = append([]MemberID(nil), s.Outside...)
	out.Familiarity = append([]familiarity(nil), s.Familiarity...)
	return out
}
