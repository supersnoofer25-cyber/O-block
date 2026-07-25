package campaign

import "errors"

// The errors Apply can return. Every one is a decision that could not be taken —
// none is a campaign outcome. Losing a member is not among them, because a death is
// not a failure (story 24, ADR 0002).
var (
	ErrEnded      = errors.New("campaign has ended")
	ErrNotAMember = errors.New("not a member of the set")
	ErrDead       = errors.New("member is dead")
	ErrGoneInside = errors.New("member has gone inside for the night")
	ErrSeat       = errors.New("seat must be riding or on-the-back")
)

// Decision is something the player does. There are exactly two, which is the whole
// choice architecture (ADR 0003): who to spend an evening with, and going out —
// with which seat and which companion.
//
// A closed interface, so no caller outside this package can invent a third.
type Decision interface{ decision() }

// SpendEvening is time outside with one member: under a car with Marcus, at the hoop
// with Omar. Not a conversation — doing the thing he does, where he does it.
type SpendEvening struct {
	With MemberID `json:"with"`
}

// BipOut is going out, and it ends the chapter.
//
// The encounter is not simulated here. Its outcome arrives as CompanionReturned —
// the module consumes results and never produces them (story 15). Whether a man comes
// home is decided by play, which is what makes ADR 0001's promise true: playing well
// saves the person in front of you.
type BipOut struct {
	Seat      Seat     `json:"seat"`
	Companion MemberID `json:"companion"`

	// CompanionReturned is the encounter's result, handed in from outside.
	CompanionReturned bool `json:"companion_returned"`
}

func (SpendEvening) decision() {}
func (BipOut) decision()       {}

// Apply is the seam: current state in, one decision in, new state out.
//
// Pure — it never mutates its argument and depends on nothing but its inputs, which
// is what makes story 28 hold. Identical decision sequences reproduce identical state
// because there is nothing else for the result to depend on.
func Apply(s State, d Decision) (State, error) {
	if !s.Running() {
		return s, ErrEnded
	}
	switch d := d.(type) {
	case SpendEvening:
		return applySpendEvening(s, d)
	case BipOut:
		return applyBipOut(s, d)
	default:
		return s, errors.New("unknown decision")
	}
}

func applySpendEvening(s State, d SpendEvening) (State, error) {
	if _, ok := MemberByID(d.With); !ok {
		return s, ErrNotAMember
	}
	if !s.IsAlive(d.With) {
		return s, ErrDead
	}
	if !s.IsOutside(d.With) {
		return s, ErrGoneInside
	}

	out := s.clone()

	// Only this member's familiarity moves. Never the set's (story 3).
	out.setEvenings(d.With, out.Evenings(d.With)+1)

	// He goes in, and so do some others — you were with him, so you were not there
	// when they left (ADR 0012). Deterministic, and never announced.
	out.Outside = remove(out.Outside, d.With)
	out.Outside = depart(out.Outside, d.With)

	return out, nil
}

// depart sends DepartAlongside members inside, choosing them deterministically: the
// ones standing after the member Jamal chose, in the set's order, wrapping around.
//
// Deterministic is required — story 28 needs identical inputs to reproduce identical
// state, and randomising this would make permanence an artefact of the save system
// rather than a rule. Deterministic is not the same as legible: a first-time player
// has no way to know the pattern, and autosave-only denies them the repetition that
// would teach it (ADR 0012).
func depart(outside []MemberID, after MemberID) []MemberID {
	if len(outside) == 0 {
		return outside
	}
	ordered := inSetOrder(outside)

	start := 0
	for i, m := range TheSet {
		if m.ID == after {
			start = i
			break
		}
	}

	leaving := map[MemberID]bool{}
	for i := 1; i <= len(TheSet) && len(leaving) < DepartAlongside; i++ {
		candidate := TheSet[(start+i)%len(TheSet)].ID
		if contains(ordered, candidate) {
			leaving[candidate] = true
		}
	}

	staying := make([]MemberID, 0, len(ordered))
	for _, m := range ordered {
		if !leaving[m] {
			staying = append(staying, m)
		}
	}
	return staying
}

func applyBipOut(s State, d BipOut) (State, error) {
	if d.Seat != Riding && d.Seat != OnBack {
		return s, ErrSeat
	}
	if _, ok := MemberByID(d.Companion); !ok {
		return s, ErrNotAMember
	}
	// Only the living can be brought (story 12). Note there is no check that he is
	// still outside: who fills the seat is not limited by who you happened to catch
	// this evening.
	if !s.IsAlive(d.Companion) {
		return s, ErrDead
	}

	out := s.clone()

	// The cycle advances on completion, whatever happened (stories 16, 17), and
	// nothing anywhere reduces it (story 18). A bip won cleanly, losing nobody,
	// still escalates.
	out.Cycle++

	// The result the encounter handed us becomes permanent. No revival, no
	// replacement, and no path back onto the roster (stories 22, 23).
	if !d.CompanionReturned {
		out.Alive = remove(out.Alive, d.Companion)
	}

	out.Chapter++

	switch {
	case len(out.Alive) == 0:
		// The loop cannot run with nobody to fill the other seat, so the campaign
		// reaches its ending early. This is arrival, not failure (ADR 0007).
		out.End = SetEmpty
		out.Outside = nil
	case out.Chapter > Chapters:
		out.End = Finished
		out.Outside = nil
	default:
		// A new evening. Everyone living is outside when a chapter opens.
		out.Outside = append([]MemberID(nil), out.Alive...)
	}

	return out, nil
}
