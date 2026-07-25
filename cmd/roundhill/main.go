// Command roundhill plays a campaign in the terminal.
//
// This is a prototype harness, not the game. There are no graphics, no bike and no
// shooting — it exists to answer one question cheaply: does the rhythm of choosing
// who to spend an evening with, and choosing who to take, hold up over sixteen
// chapters? That is the bet ADRs 0008 and 0013 make and cannot otherwise test until
// something is playable.
//
// It deliberately honours the foreclosures, so what you feel here is the real design:
//
//   - No familiarity meter. ADR 0006 and ADR 0012 forbid surfacing it. You know who
//     you have spent evenings with because you remember, which is the whole point.
//   - No chapter count and no progress bar. ADR 0008 forbids them.
//   - No survivor tally at the ending. ADR 0007 forbids it.
//   - Nothing frames a death as failure. ADR 0002 forbids it.
//
// The encounter is not simulated. This harness rolls for whether a companion comes
// home and hands the result to the module, which is exactly the boundary issue #3
// draws — in the real game that result comes from how the player played.
package main

import (
	"bufio"
	"flag"
	"fmt"
	"math/rand"
	"os"
	"strings"

	"github.com/supersnoofer25-cyber/O-block/campaign"
)

func main() {
	auto := flag.Bool("auto", false, "play it through without asking, to watch a whole campaign")
	seed := flag.Int64("seed", 1, "seed for whether companions come home")
	lossRate := flag.Int("loss", 4, "one companion lost in N bips, the tuning target")
	flag.Parse()

	g := &game{
		state: campaign.New(),
		rng:   rand.New(rand.NewSource(*seed)),
		in:    bufio.NewScanner(os.Stdin),
		auto:  *auto,
		loss:  *lossRate,
	}
	g.run()
}

type game struct {
	state campaign.State
	rng   *rand.Rand
	in    *bufio.Scanner
	auto  bool
	loss  int
}

func (g *game) run() {
	fmt.Println()
	fmt.Println("ROUNDHILL")
	fmt.Println("You are Jamal. These are the people you grew up with.")
	fmt.Println()

	for g.state.Running() {
		g.evening()
		g.bip()
	}
	g.ending()
}

// evening is time outside: the player spends it until there is nobody left out.
func (g *game) evening() {
	for len(g.state.Outside) > 0 {
		fmt.Println("Outside tonight —")
		for _, id := range g.state.Outside {
			m, _ := campaign.MemberByID(id)
			fmt.Printf("  %-10s %s\n", m.Name, lower(m.Activity))
		}

		with := g.pick("Who do you spend the evening with?", g.state.Outside)
		before := append([]campaign.MemberID(nil), g.state.Outside...)

		next, err := campaign.Apply(g.state, campaign.SpendEvening{With: with})
		if err != nil {
			fmt.Println("  ", err)
			continue
		}
		g.state = next

		m, _ := campaign.MemberByID(with)
		fmt.Printf("\nYou spend the evening with %s, %s.\n", m.Name, lower(m.Activity))

		// The others carried on without you. Nothing was deducted — you were
		// somewhere else (ADR 0012).
		if gone := missing(before, g.state.Outside, with); len(gone) > 0 {
			fmt.Printf("%s went in while you were there.\n", names(gone))
		}
		fmt.Println()
	}
	fmt.Println("The Block is quiet. There's nobody left out.")
	fmt.Println()
}

// bip is going out, which ends the chapter.
func (g *game) bip() {
	seat := campaign.Riding
	if g.ask("Do you ride, or take the back?", "ride", "back") == "back" {
		seat = campaign.OnBack
	}

	fmt.Println()
	for _, id := range g.state.Companions() {
		m, _ := campaign.MemberByID(id)
		how := m.Capability.OnBack
		if seat == campaign.OnBack {
			how = m.Capability.Riding
		}
		fmt.Printf("  %-10s %s. %s.\n", m.Name, m.Capability.Name, how)
	}
	prompt := "Who's on the back?"
	if seat == campaign.OnBack {
		prompt = "Who rides?"
	}
	companion := g.pick(prompt, g.state.Companions())

	// The harness decides whether he comes home; the module only records it.
	// In the real game this is the player's own play (ADR 0001).
	returned := g.rng.Intn(g.loss) != 0

	m, _ := campaign.MemberByID(companion)
	fmt.Println()
	if seat == campaign.Riding {
		fmt.Printf("You ride. %s takes the back.\n", m.Name)
	} else {
		fmt.Printf("%s rides. You take the back.\n", m.Name)
	}

	next, err := campaign.Apply(g.state, campaign.BipOut{
		Seat: seat, Companion: companion, CompanionReturned: returned,
	})
	if err != nil {
		fmt.Println("  ", err)
		return
	}
	g.state = next

	fmt.Println()
	if returned {
		fmt.Printf("You both come back.\n")
	} else {
		// Never framed as failure. The story absorbs it (ADR 0002).
		fmt.Printf("%s doesn't come home.\n\n%s\n", m.Name, m.CostOfLoss)
		if tie, ok := campaign.MemberByID(m.Tie); ok && g.state.IsAlive(tie.ID) {
			fmt.Printf("%s doesn't say anything.\n", tie.Name)
		}
	}
	fmt.Println()
	fmt.Println(strings.Repeat("—", 60))
	fmt.Println()
}

func (g *game) ending() {
	// ADR 0007 forecloses a count, a score, a percentage, and a list of who made it.
	// So nothing here enumerates the survivors — the player knows who is standing
	// next to them, because they were the one choosing. What the ending is made of
	// is what is missing, which needs no additional system: the accumulated costs of
	// loss are the Block.
	fmt.Println()
	if len(g.state.Alive) == 0 {
		fmt.Println("There's nobody left to go with.")
	}
	for _, m := range campaign.TheSet {
		if !g.state.IsAlive(m.ID) {
			fmt.Println(m.CostOfLoss)
		}
	}
	fmt.Println()
	fmt.Println("The beef is where it was, and worse.")
}

// --- input -----------------------------------------------------------------

func (g *game) pick(prompt string, from []campaign.MemberID) campaign.MemberID {
	if g.auto {
		choice := from[g.rng.Intn(len(from))]
		m, _ := campaign.MemberByID(choice)
		fmt.Printf("%s %s\n", prompt, m.Name)
		return choice
	}
	for {
		fmt.Printf("%s ", prompt)
		line := g.readLine()
		for _, id := range from {
			m, _ := campaign.MemberByID(id)
			if strings.EqualFold(line, m.Name) || strings.EqualFold(line, string(id)) {
				return id
			}
		}
		fmt.Println("  Nobody by that name is out. Try again.")
	}
}

func (g *game) ask(prompt string, options ...string) string {
	if g.auto {
		choice := options[g.rng.Intn(len(options))]
		fmt.Printf("%s %s\n", prompt, choice)
		return choice
	}
	for {
		fmt.Printf("%s [%s] ", prompt, strings.Join(options, "/"))
		line := g.readLine()
		for _, o := range options {
			if strings.EqualFold(line, o) {
				return o
			}
		}
	}
}

func (g *game) readLine() string {
	if !g.in.Scan() {
		fmt.Println()
		os.Exit(0)
	}
	return strings.TrimSpace(g.in.Text())
}

// --- formatting ------------------------------------------------------------

func missing(before, after []campaign.MemberID, except campaign.MemberID) []campaign.MemberID {
	var gone []campaign.MemberID
	for _, id := range before {
		if id == except {
			continue
		}
		found := false
		for _, s := range after {
			if s == id {
				found = true
				break
			}
		}
		if !found {
			gone = append(gone, id)
		}
	}
	return gone
}

func names(ids []campaign.MemberID) string {
	var out []string
	for _, id := range ids {
		m, _ := campaign.MemberByID(id)
		out = append(out, m.Name)
	}
	switch len(out) {
	case 0:
		return ""
	case 1:
		return out[0]
	default:
		return strings.Join(out[:len(out)-1], ", ") + " and " + out[len(out)-1]
	}
}

func lower(s string) string {
	if s == "" {
		return s
	}
	return strings.ToLower(s[:1]) + s[1:]
}
