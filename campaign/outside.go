package campaign

// Ambient content for time outside: where each member is tonight, and why he goes in
// when he goes.
//
// ADR 0012 asks for this per chapter and not once — "each member also needs somewhere
// to be and a plausible reason to go in, per chapter" — and names the line it has to
// walk: departures must be legible enough to feel like people going home and opaque
// enough never to become a schedule. cast.go carries one fixed Activity per man, which
// is the whole of what a campaign had to vary itself with, and a campaign of it reads
// as one chapter repeated.
//
// This is not a second property of a member. ADR 0009 fixes a member at one capability
// and one cost of loss, and nothing here may rank anyone or be derived from either.
// Every line stays inside the domain the man is already in — Marcus is under a car in
// all of his, and which car is not a fact about Marcus. That is deliberate: if these
// lines characterised, they would be a third thing a member carries, and the roster
// would have gained an axis while nobody was looking.
//
// Lookups are by key, so the map is not an ordering and story 28 is unaffected.

// tonight is where each man is, in a phrase that slots in after his name.
var tonight = map[MemberID][]string{
	Alex: {
		"on the steps, his sister doing homework beside him",
		"walking his sister back from the shop",
		"on the wall, his sister and two of her friends round him",
		"outside his building while his sister skips rope",
		"on the kerb, his sister arguing with someone about nothing",
	},
	Tyrone: {
		"at his spot, working a queue that isn't moving",
		"on the corner arguing prices with someone who isn't buying",
		"at his spot with nothing shifting, talking anyway",
		"leaning on the shutter, holding court",
		"at his spot, packing up slow",
	},
	Anthony: {
		"in his work clothes, not changed yet",
		"before his shift, killing twenty minutes",
		"just off, still smelling of the job",
		"on the wall with his bag between his feet",
		"waiting on a lift to work that hasn't come",
	},
	James: {
		"up on the church roof with a bucket",
		"on the roof, sat down, not working",
		"round the back of the church with the ladder",
		"up top, looking at the same patch as last week",
		"on the church steps with dust all over him",
	},
	Daquavion: {
		"on his step with the baby on his shoulder",
		"outside his place, rocking the pram with his foot",
		"in the doorway while the baby sleeps inside",
		"walking the baby up and down to settle her",
		"on the wall, checking his phone every minute",
	},
	Marcus: {
		"under a car with only his legs showing",
		"bent into somebody's engine",
		"under the same car he was under last time",
		"jump-starting something for whoever asked",
		"wiping his hands on a rag, done for now",
	},
	Omar: {
		"at the hoop, taking the same shot over",
		"at the hoop with nobody to play",
		"sat on the ball, not playing",
		"playing whoever turned up",
		"at the hoop long after everyone else went in",
	},
	Joshua: {
		"on your step, because he lives there",
		"in the doorway of your house",
		"on the wall outside yours",
		"at home, waiting for you to get back",
		"on the step with the door open behind him",
	},
}

// wentIn is why he goes in, phrased so it reads as an evening ending rather than as a
// rule firing. ADR 0012's failure mode is a player feeling robbed instead of rushed.
var wentIn = map[MemberID][]string{
	Alex:      {"takes his sister up", "gets called in by his mum", "walks his sister home and doesn't come back"},
	Tyrone:    {"counts up and goes", "packs it in, nothing moving", "is done for the night"},
	Anthony:   {"has work", "goes up to sleep before his shift", "gets his lift at last"},
	James:     {"climbs down when the light goes", "can't see what he's doing any more", "puts the ladder round the back"},
	Daquavion: {"takes the baby in", "gets called up by his girl", "goes in when she finally goes down"},
	Marcus:    {"goes to wash his hands", "runs out of light", "goes in to eat"},
	Omar:      {"gets called in", "has had enough", "goes in when it gets cold"},
	Joshua:    {"goes up — it's his house too", "is inside before you are", "shuts the door behind him"},
}

// Tonight is where a member is this evening.
//
// The chapter selects the line, and the campaign never shows the player a chapter
// number (ADR 0008) — this is the number being used to author against, not to display.
func Tonight(id MemberID, chapter int) string {
	lines := tonight[id]
	if len(lines) == 0 {
		m, _ := MemberByID(id)
		return m.Activity
	}
	return lines[rotate(id, chapter, len(lines))]
}

// WentIn is why a member went inside, as a clause following his name.
func WentIn(id MemberID, chapter int) string {
	lines := wentIn[id]
	if len(lines) == 0 {
		return "went in"
	}
	return lines[rotate(id, chapter, len(lines))]
}

// rotate picks a line deterministically, which story 28 requires — identical decision
// sequences must reproduce identical play, and a random pick here would make what the
// player saw depend on when they saw it.
//
// Each man is offset by his place in the set so the eight do not advance in lockstep.
// Lockstep is what would turn this into the schedule ADR 0012 forecloses: the whole
// Block changing on the same beat is a pattern to read, where eight cycles of different
// lengths sliding past each other is just the Block.
func rotate(id MemberID, chapter, n int) int {
	offset := 0
	for i, m := range TheSet {
		if m.ID == id {
			offset = i
			break
		}
	}
	if chapter < 1 {
		chapter = 1
	}
	return (chapter - 1 + offset) % n
}
