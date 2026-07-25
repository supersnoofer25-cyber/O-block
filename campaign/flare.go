package campaign

import "fmt"

// A flare is why tonight.
//
// ADR 0011 generates it from the player's previous bip and forecloses every external
// source — no schedule, no escalation manager, no quest-giver — on the grounds that a
// managed beef is not the self-feeding cycle CONTEXT.md defines. So there is no system
// here. There is a lookup from what the player did last time to what the Block says
// about it now, which is the whole mechanism.
//
// Two rules from ADR 0011 are structural rather than editorial, and the code holds
// them so the prose cannot quietly drift off them:
//
//   - A flare never kills a member of the set. They die on bips, where the player put
//     them. Nothing in this file can reach the roster.
//   - A flare never touches a cost of loss. Not the eight, not their people, not their
//     places — Marcus's cars, James's church, Omar's hoop, Alex's sister, Daquavion's
//     girl and child, Tyrone's selling, Anthony's job, and the house Jamal lives in are
//     all reserved, because those eight subtractions have to stay attributable to the
//     eight deaths for ADR 0007's ending to work. Every place named below is somewhere
//     else on the Block.
//
// The tonal hazard ADR 0011 names as the hardest writing in the project after the
// ending: a death outside the set must never read as fuel. Nothing here tells the
// player to do anything about it, and nothing frames it as a reason.
type Flare struct {
	// News is how it reaches the player: outside, from people, in the wrong order and
	// more than once. Never a marker, an objective or a briefing.
	News string

	// Dead is somebody on the Block who was killed, or empty. A flare does not kill
	// every time — anonymous casualties every chapter would make death cheap and drain
	// the roster deaths of the weight everything else exists to give them.
	Dead string
}

// FlareFor is tonight's reason, read off the last bip.
//
// Pure and deterministic, like Apply and for the same reason: story 28 needs identical
// decisions to reproduce identical play, and a flare that rolled would make what the
// player was told depend on when they were told it.
func FlareFor(s State) Flare {
	f := Flare{News: news(s)}
	if killed(s) {
		f.Dead = bystanders[s.Cycle%len(bystanders)]
	}
	return f
}

// news picks the line. Kind comes from the last bip; severity comes from intensity,
// which climbs over the first third of the campaign and then sits at its ceiling
// (ADR 0013). Authoring the back ten chapters against a fixed intensity is what that
// decision asked for — the plateau is the statement, and chapter sixteen is no worse
// than chapter eight.
func news(s State) string {
	if !s.LastBip.Happened {
		return inherited
	}
	lines := answered
	if s.LastBip.Lost {
		lines = goingFor
	}
	line := lines[band(s.Intensity())]
	if s.LastBip.Lost {
		m, _ := MemberByID(s.LastBip.Who)
		return fmt.Sprintf(line, m.Name)
	}
	return line
}

// band collapses intensity into the three registers the flares are written in. The top
// one is where the campaign spends most of its length, by design.
func band(intensity int) int {
	switch {
	case intensity <= 1:
		return 0
	case intensity <= 3:
		return 1
	default:
		return 2
	}
}

// killed decides whether tonight's flare took somebody, deterministically and not
// every time.
func killed(s State) bool {
	// Not on the first, where there is no bip behind it yet, and never so often that
	// it stops landing.
	return s.LastBip.Happened && s.Cycle%3 == 2
}

// The beef is inherited, so chapter one needs no special case — its reason is simply
// the most recent thing that happened before the game began. It is also the one flare
// that cannot say why any of it started, because nothing can (ADR 0010).
const inherited = "Nobody has to tell you why. Thursday they came through twice, slow, with the windows down, and everyone out saw it. It was going before you were old enough to be in it, and no two people tell the same story about how."

// They answer a bip that cost the player nothing. He got out clean; they did not
// agree to let it go.
var answered = []string{
	"Somebody has it before you're properly out, then somebody else tells it different. They came back through in the night and did the windows along the low block. Nobody was in. Everyone knows what it was for.",
	"You get told three times before you reach the corner and it's worse each time. They came through with the lights off and put the door of the low block in.",
	"Nobody bothers telling it properly any more. They came through in the night. They'll come through again, and after that they'll come through again.",
}

// The player lost somebody last time, so the Block goes for him. One %s, his name.
//
// This is the flare that most needs to read as aggression rather than as consequence
// (ADR 0011): the causal chain is completely visible, nothing is concealed, and from
// inside it still feels like the only thing to do.
var goingFor = []string{
	"Nobody organises it. By the time you're outside it's already settled where everyone is going tonight, and it's for %s.",
	"It's decided before you're down the steps. Nobody says %s's name, and it's the only thing anybody is talking about.",
	"Nobody discusses it any more. It's for %s, the way the last one was for somebody, the way the next one will be.",
}

// People on the Block who are not in the set. Named, placed and recognisable — someone
// the player could have seen outside — because ADR 0011 rejects anonymous casualties.
// None of these places or people is part of anybody's cost of loss.
var bystanders = []string{
	"Renée, who works the launderette counter, was on the pavement when it happened. She had nothing to do with any of it.",
	"Curtis got hit at the bus stop. He was waiting for a bus.",
	"Miss Pat, top floor of the low block, was at her window. Nobody on the Block says very much about it.",
}
