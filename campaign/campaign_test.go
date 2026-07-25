package campaign

import (
	"encoding/json"
	"os"
	"reflect"
	"testing"
)

// One test per [seam] story in issue #3, named so a failure names the story it broke.
// The [playtest] stories are deliberately absent: encounter feel, bike handling, hub
// traversal and companion AI are judged by playing, and automating them would bolt
// tests to an implementation expected to change.

// --- helpers ---------------------------------------------------------------

// must(t)(Apply(...)) asserts a decision was legal and returns the new state.
func must(t *testing.T) func(State, error) State {
	return func(s State, err error) State {
		t.Helper()
		if err != nil {
			t.Fatalf("unexpected error: %v", err)
		}
		return s
	}
}

// spendAll takes every evening the chapter will give, then goes out.
func spendAll(t *testing.T, s State, companionReturns bool) State {
	t.Helper()
	for len(s.Outside) > 0 {
		s = must(t)(Apply(s, SpendEvening{With: s.Outside[0]}))
	}
	return must(t)(Apply(s, BipOut{
		Seat:              Riding,
		Companion:         s.Alive[0],
		CompanionReturned: companionReturns,
	}))
}

// --- Outside ---------------------------------------------------------------

func TestStory1_EightMembersAtStart(t *testing.T) {
	s := New()
	if len(TheSet) != 8 {
		t.Fatalf("the set has %d members, want 8", len(TheSet))
	}
	if len(s.Alive) != 8 {
		t.Fatalf("campaign starts with %d alive, want 8", len(s.Alive))
	}
	seen := map[MemberID]bool{}
	for _, m := range TheSet {
		if seen[m.ID] {
			t.Fatalf("duplicate member %q", m.ID)
		}
		seen[m.ID] = true
	}
}

func TestStory2_EveningsAreFiniteAndTheOnlyWayToKnowSomeone(t *testing.T) {
	s := New()

	// The chapter is finite: evenings run out.
	spent := 0
	for len(s.Outside) > 0 {
		s = must(t)(Apply(s, SpendEvening{With: s.Outside[0]}))
		spent++
		if spent > 8 {
			t.Fatal("a chapter never ran out of evenings")
		}
	}
	if spent == 0 {
		t.Fatal("a chapter gave no evenings at all")
	}

	// Going out is not a way to get to know anyone.
	before := s.Evenings(Marcus)
	s = must(t)(Apply(s, BipOut{Seat: OnBack, Companion: Marcus, CompanionReturned: true}))
	if got := s.Evenings(Marcus); got != before {
		t.Fatalf("a bip changed familiarity with Marcus: %d -> %d", before, got)
	}
}

func TestStory3_EveningsReachOneManOnly(t *testing.T) {
	s := New()
	before := map[MemberID]int{}
	for _, m := range TheSet {
		before[m.ID] = s.Evenings(m.ID)
	}

	s = must(t)(Apply(s, SpendEvening{With: Marcus}))

	for _, m := range TheSet {
		got, want := s.Evenings(m.ID), before[m.ID]
		if m.ID == Marcus {
			want++
		}
		if got != want {
			t.Errorf("%s: evenings %d, want %d", m.ID, got, want)
		}
	}
}

func TestStory4_CannotKnowAllEightWell(t *testing.T) {
	// Arithmetic first: the bound holds before anyone has even died.
	maxPerChapter := (len(TheSet) + DepartAlongside) / (1 + DepartAlongside)
	if maxPerChapter*Chapters >= len(TheSet)*EveningsToClose {
		t.Fatalf("a campaign yields %d evenings; knowing all eight well takes %d",
			maxPerChapter*Chapters, len(TheSet)*EveningsToClose)
	}

	// And in play, against a player doing the best possible job of spreading their
	// evenings, with nobody dying — the most generous campaign there is.
	s := New()
	for s.Running() {
		for len(s.Outside) > 0 {
			least := s.Outside[0]
			for _, id := range s.Outside {
				if s.Evenings(id) < s.Evenings(least) {
					least = id
				}
			}
			s = must(t)(Apply(s, SpendEvening{With: least}))
		}
		s = must(t)(Apply(s, BipOut{Seat: Riding, Companion: s.Alive[0], CompanionReturned: true}))
	}
	close := 0
	for _, m := range TheSet {
		if s.Knows(m.ID) == Close {
			close++
		}
	}
	if close == len(TheSet) {
		t.Fatal("a perfect campaign came to know all eight well")
	}
}

func TestStory5_FamiliarityIsNeverLost(t *testing.T) {
	s := New()
	s = must(t)(Apply(s, SpendEvening{With: Marcus}))
	// Tyrone is one of the few still out after an evening with Marcus — the others
	// went in while Jamal was under a car.
	s = must(t)(Apply(s, SpendEvening{With: Tyrone}))
	marcus, tyrone := s.Evenings(Marcus), s.Evenings(Tyrone)

	// Tyrone does not come home. Marcus is untouched, and so is Tyrone's own history.
	s = must(t)(Apply(s, BipOut{Seat: Riding, Companion: Tyrone, CompanionReturned: false}))
	if s.IsAlive(Tyrone) {
		t.Fatal("Tyrone survived a bip he did not return from")
	}
	if got := s.Evenings(Marcus); got != marcus {
		t.Errorf("Marcus lost familiarity when Tyrone died: %d -> %d", marcus, got)
	}
	if got := s.Evenings(Tyrone); got != tyrone {
		t.Errorf("Tyrone's familiarity was reset by his death: %d -> %d", tyrone, got)
	}
}

// --- Preparing the ride ----------------------------------------------------

func TestStory8_SeatIsChosenEveryBip(t *testing.T) {
	s := New()
	if _, err := Apply(s, BipOut{Companion: Marcus, CompanionReturned: true}); err != ErrSeat {
		t.Fatalf("a bip with no seat was allowed: %v", err)
	}
	for _, seat := range []Seat{Riding, OnBack} {
		if _, err := Apply(s, BipOut{Seat: seat, Companion: Marcus, CompanionReturned: true}); err != nil {
			t.Errorf("seat %q rejected: %v", seat, err)
		}
	}
}

func TestStory9_SeatIsNeverLockedInByTheLastOne(t *testing.T) {
	s := New()
	s = must(t)(Apply(s, BipOut{Seat: Riding, Companion: Marcus, CompanionReturned: true}))
	// The opposite seat is available immediately, and so is the same one again.
	if _, err := Apply(s, BipOut{Seat: OnBack, Companion: Marcus, CompanionReturned: true}); err != nil {
		t.Errorf("could not switch seat after riding: %v", err)
	}
	if _, err := Apply(s, BipOut{Seat: Riding, Companion: Marcus, CompanionReturned: true}); err != nil {
		t.Errorf("could not keep the same seat: %v", err)
	}
}

func TestStory10_ExactlyOneMemberFillsTheOtherSeat(t *testing.T) {
	s := New()
	if _, err := Apply(s, BipOut{Seat: Riding, CompanionReturned: true}); err != ErrNotAMember {
		t.Fatalf("a bip with no companion was allowed: %v", err)
	}
	if _, err := Apply(s, BipOut{Seat: Riding, Companion: "nobody", CompanionReturned: true}); err != ErrNotAMember {
		t.Fatalf("a bip with an invented companion was allowed: %v", err)
	}
}

func TestStory11_RideIsCappedByTheBikeNotByARule(t *testing.T) {
	// The cap is structural: BipOut has one companion field and it is not a
	// collection, so a second passenger cannot be expressed. There is no number to
	// raise and no rule to relax.
	f, ok := reflect.TypeOf(BipOut{}).FieldByName("Companion")
	if !ok {
		t.Fatal("BipOut has no Companion field")
	}
	if k := f.Type.Kind(); k == reflect.Slice || k == reflect.Array || k == reflect.Map {
		t.Fatalf("Companion is a %s, so more than one could ride", k)
	}
	for i := 0; i < reflect.TypeOf(BipOut{}).NumField(); i++ {
		if name := reflect.TypeOf(BipOut{}).Field(i).Name; name == "Companions" || name == "Riders" {
			t.Fatalf("BipOut has a %q field, which would uncap the ride", name)
		}
	}
}

func TestStory12_OnlyTheLivingCanBeBrought(t *testing.T) {
	s := New()
	s = must(t)(Apply(s, BipOut{Seat: Riding, Companion: Omar, CompanionReturned: false}))
	if _, err := Apply(s, BipOut{Seat: Riding, Companion: Omar, CompanionReturned: true}); err != ErrDead {
		t.Fatalf("a dead man was brought on a bip: %v", err)
	}
	for _, id := range s.Companions() {
		if !s.IsAlive(id) {
			t.Fatalf("%s is offered as a companion but is dead", id)
		}
	}
}

func TestStory13_NothingCanGrowTheSet(t *testing.T) {
	// Behavioural: no sequence of decisions makes the roster longer.
	s := New()
	start := len(s.Alive)
	for s.Running() {
		s = spendAll(t, s, s.Cycle%4 != 0) // lose someone every fourth bip
		if len(s.Alive) > start {
			t.Fatalf("the roster grew to %d from %d", len(s.Alive), start)
		}
	}
	// Structural: the roster can only ever be drawn from the original eight.
	for _, id := range s.Alive {
		if _, ok := MemberByID(id); !ok {
			t.Fatalf("%s is alive but is not one of the eight", id)
		}
	}
}

func TestStory14_MemberCarriesNoNumber(t *testing.T) {
	// Every field on a member is a string. With nothing numeric, two members cannot
	// be ordered, so the roster cannot become a comparison of statistics.
	var check func(t reflect.Type, path string)
	check = func(typ reflect.Type, path string) {
		for i := 0; i < typ.NumField(); i++ {
			f := typ.Field(i)
			switch f.Type.Kind() {
			case reflect.String:
			case reflect.Struct:
				check(f.Type, path+"."+f.Name)
			default:
				t.Errorf("%s.%s is a %s; a member may carry no number", path, f.Name, f.Type.Kind())
			}
		}
	}
	check(reflect.TypeOf(Member{}), "Member")

	// And what selecting a member exposes is who he is.
	m, _ := MemberByID(Daquavion)
	if m.CostOfLoss == "" || m.Capability.Riding == "" || m.Capability.OnBack == "" {
		t.Fatal("a member does not expose who he is in both seats and what his loss costs")
	}
}

// --- Bipping out -----------------------------------------------------------

func TestStory15_EncounterOutcomeEntersAsAResult(t *testing.T) {
	// The module consumes the result it is handed and never decides it. The same
	// decision with opposite outcomes must produce opposite rosters.
	base := New()
	lived := must(t)(Apply(base, BipOut{Seat: Riding, Companion: Marcus, CompanionReturned: true}))
	died := must(t)(Apply(base, BipOut{Seat: Riding, Companion: Marcus, CompanionReturned: false}))

	if !lived.IsAlive(Marcus) {
		t.Error("Marcus returned but was recorded dead")
	}
	if died.IsAlive(Marcus) {
		t.Error("Marcus did not return but was recorded alive")
	}
}

func TestStory16_EveryBipEscalatesWhateverHappened(t *testing.T) {
	for _, returned := range []bool{true, false} {
		s := New()
		before := s.Cycle
		s = must(t)(Apply(s, BipOut{Seat: OnBack, Companion: James, CompanionReturned: returned}))
		if s.Cycle <= before {
			t.Errorf("companion returned=%v: cycle did not advance (%d -> %d)", returned, before, s.Cycle)
		}
	}
}

func TestStory17_WinningCleanlyStillEscalates(t *testing.T) {
	s := New()
	for i := 0; i < 5; i++ {
		before := s.Cycle
		s = must(t)(Apply(s, BipOut{Seat: Riding, Companion: Anthony, CompanionReturned: true}))
		if s.Cycle != before+1 {
			t.Fatalf("clean bip %d did not escalate: %d -> %d", i+1, before, s.Cycle)
		}
	}
	if len(s.Alive) != 8 {
		t.Fatal("nobody should have died")
	}
	if s.Cycle != 5 {
		t.Fatalf("cycle is %d after five clean bips, want 5", s.Cycle)
	}
}

func TestStory18_EscalationIsMonotonic(t *testing.T) {
	s := New()
	last := s.Cycle
	lastIntensity := s.Intensity()
	for s.Running() {
		s = spendAll(t, s, s.Cycle%3 != 0)
		if s.Cycle < last {
			t.Fatalf("cycle went backwards: %d -> %d", last, s.Cycle)
		}
		if s.Intensity() < lastIntensity {
			t.Fatalf("intensity went backwards: %d -> %d", lastIntensity, s.Intensity())
		}
		last, lastIntensity = s.Cycle, s.Intensity()
	}
}

func TestIntensityPlateaus(t *testing.T) {
	// ADR 0013: escalation climbs early and then stops. Chapter sixteen is no more
	// intense than chapter eight, even though the cycle has kept advancing.
	s := New()
	for s.Running() {
		s = spendAll(t, s, true)
	}
	if s.Intensity() != IntensityCeiling {
		t.Fatalf("intensity is %d at the end, want the ceiling %d", s.Intensity(), IntensityCeiling)
	}
	if s.Cycle <= IntensityCeiling {
		t.Fatal("the cycle should have run well past the intensity ceiling")
	}
}

// --- Loss and persistence --------------------------------------------------

func TestStory22_DeathIsPermanent(t *testing.T) {
	s := New()
	s = must(t)(Apply(s, BipOut{Seat: Riding, Companion: Tyrone, CompanionReturned: false}))
	if s.IsAlive(Tyrone) {
		t.Fatal("Tyrone is alive after not returning")
	}
	for s.Running() {
		s = spendAll(t, s, true)
		if s.IsAlive(Tyrone) {
			t.Fatal("Tyrone came back")
		}
	}
}

func TestStory23_RosterOnlyShortens(t *testing.T) {
	s := New()
	last := len(s.Alive)
	for s.Running() {
		s = spendAll(t, s, s.Cycle%2 == 0)
		if len(s.Alive) > last {
			t.Fatalf("roster grew: %d -> %d", last, len(s.Alive))
		}
		last = len(s.Alive)
	}
}

func TestStory24_DeathIsNeverAFailState(t *testing.T) {
	// Losing someone is not an error, and it produces no terminal outcome.
	s := New()
	next, err := Apply(s, BipOut{Seat: Riding, Companion: Joshua, CompanionReturned: false})
	if err != nil {
		t.Fatalf("losing a man returned an error: %v", err)
	}
	if !next.Running() {
		t.Fatal("losing a man ended the campaign")
	}

	// And there is no failure outcome to reach: kill everyone and the ending is
	// still an ending.
	s = next
	for s.Running() {
		s = must(t)(Apply(s, BipOut{Seat: Riding, Companion: s.Alive[0], CompanionReturned: false}))
	}
	if s.End != SetEmpty {
		t.Fatalf("ended as %q, want %q", s.End, SetEmpty)
	}
	if len(s.Alive) != 0 {
		t.Fatal("the set should be empty")
	}
}

func TestStory25_PlayableWithAnySubsetAlive(t *testing.T) {
	// Every ordering of losses must leave a campaign that still runs. Walk each
	// member out first and check the rest still works.
	for _, first := range TheSet {
		s := New()
		s = must(t)(Apply(s, BipOut{Seat: OnBack, Companion: first.ID, CompanionReturned: false}))
		steps := 0
		for s.Running() {
			s = spendAll(t, s, len(s.Alive) > 1)
			steps++
			if steps > Chapters+2 {
				t.Fatalf("losing %s first produced a campaign that would not end", first.ID)
			}
		}
		if s.End == NotEnded {
			t.Fatalf("losing %s first left the campaign hanging", first.ID)
		}
	}
}

func TestStory26_StateRoundTrips(t *testing.T) {
	s := New()
	s = must(t)(Apply(s, SpendEvening{With: Marcus}))
	s = must(t)(Apply(s, BipOut{Seat: OnBack, Companion: Alex, CompanionReturned: false}))

	dir := t.TempDir()
	if err := Save(dir, s); err != nil {
		t.Fatalf("save: %v", err)
	}
	back, err := Load(dir)
	if err != nil {
		t.Fatalf("load: %v", err)
	}
	if !reflect.DeepEqual(s, back) {
		t.Fatalf("state did not survive the round trip:\n got %+v\nwant %+v", back, s)
	}

	// And it resumes: the next decision behaves identically either way.
	a := must(t)(Apply(s, SpendEvening{With: Omar}))
	b := must(t)(Apply(back, SpendEvening{With: Omar}))
	if !reflect.DeepEqual(a, b) {
		t.Fatal("a restored campaign did not resume where it stopped")
	}
}

func TestStory27_OneSlotThatAdvancesOnItsOwn(t *testing.T) {
	dir := t.TempDir()
	s := New()
	for i := 0; i < 4; i++ {
		if len(s.Outside) > 0 {
			s = must(t)(Apply(s, SpendEvening{With: s.Outside[0]}))
		} else {
			s = must(t)(Apply(s, BipOut{Seat: Riding, Companion: s.Alive[0], CompanionReturned: true}))
		}
		if err := Save(dir, s); err != nil {
			t.Fatalf("save: %v", err)
		}
	}
	entries, err := os.ReadDir(dir)
	if err != nil {
		t.Fatal(err)
	}
	if len(entries) != 1 || entries[0].Name() != SlotName {
		var names []string
		for _, e := range entries {
			names = append(names, e.Name())
		}
		t.Fatalf("saving four times left %v, want exactly [%s]", names, SlotName)
	}

	// No API exists for naming a slot or restoring an earlier one: Save and Load
	// take a directory and nothing else. If either grew a slot argument, this fails.
	if n := reflect.TypeOf(Save).NumIn(); n != 2 {
		t.Errorf("Save takes %d arguments, want 2 (dir, state)", n)
	}
	if n := reflect.TypeOf(Load).NumIn(); n != 1 {
		t.Errorf("Load takes %d arguments, want 1 (dir)", n)
	}
}

func TestStory28_IdenticalDecisionsReproduceIdenticalState(t *testing.T) {
	// A legal sequence: each evening is with someone still outside, which is itself
	// a consequence of who was chosen before them.
	decisions := []Decision{
		SpendEvening{With: Marcus},
		SpendEvening{With: Tyrone},
		BipOut{Seat: Riding, Companion: Daquavion, CompanionReturned: false},
		SpendEvening{With: Omar},
		BipOut{Seat: OnBack, Companion: Tyrone, CompanionReturned: true},
		SpendEvening{With: Anthony},
		BipOut{Seat: Riding, Companion: James, CompanionReturned: false},
	}
	run := func() State {
		s := New()
		for _, d := range decisions {
			s = must(t)(Apply(s, d))
		}
		return s
	}

	// Same sequence twice.
	first, second := run(), run()
	a, _ := json.Marshal(first)
	b, _ := json.Marshal(second)
	if string(a) != string(b) {
		t.Fatalf("the same decisions produced different states:\n%s\n%s", a, b)
	}

	// And across a save: stop halfway, reload, finish. Same answer.
	dir := t.TempDir()
	s := New()
	for _, d := range decisions[:3] {
		s = must(t)(Apply(s, d))
	}
	if err := Save(dir, s); err != nil {
		t.Fatal(err)
	}
	resumed, err := Load(dir)
	if err != nil {
		t.Fatal(err)
	}
	for _, d := range decisions[3:] {
		resumed = must(t)(Apply(resumed, d))
	}
	c, _ := json.Marshal(resumed)
	if string(c) != string(a) {
		t.Fatalf("resuming from a save diverged:\n%s\n%s", c, a)
	}
}

// --- Done looks like -------------------------------------------------------

func TestFullCampaignRunsWithNoEngine(t *testing.T) {
	// #3's "done looks like": a whole campaign driven start to finish through the
	// interface, with nothing rendering and no engine present.
	s := New()
	chapters := 0
	for s.Running() {
		// Lose someone roughly one bip in four, which is the tuning target.
		s = spendAll(t, s, (s.Cycle+1)%4 != 0)
		chapters++
	}
	if chapters != Chapters {
		t.Fatalf("campaign ran %d chapters, want %d", chapters, Chapters)
	}
	if s.End != Finished {
		t.Fatalf("campaign ended as %q, want %q", s.End, Finished)
	}
	if len(s.Alive) == 0 || len(s.Alive) == 8 {
		t.Fatalf("at one bip in four the set should be diminished, not %d of 8", len(s.Alive))
	}
	t.Logf("finished chapter %d with %d of 8 alive, cycle %d, intensity %d",
		Chapters, len(s.Alive), s.Cycle, s.Intensity())
}
