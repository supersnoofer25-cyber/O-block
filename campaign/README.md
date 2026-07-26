# campaign

The headless, engine-agnostic rules module for *Our Block*. Implements
[issue #3](https://github.com/supersnoofer25-cyber/O-block/issues/3).

No engine is chosen and none is needed. Nothing here renders, reads input, or imports
anything outside the standard library.

```
go test ./...
```

## The seam

One function.

```go
func Apply(s State, d Decision) (State, error)
```

Current state in, one player decision in, new state out. There are exactly two
decisions, which is the whole choice architecture ([ADR 0003](../docs/adr/0003-choice-lives-in-systems-not-dialogue.md)):

- `SpendEvening{With}` — time **outside** with one member, doing the thing he does
- `BipOut{Seat, Companion, CompanionReturned}` — going out, which ends the chapter

`Apply` is pure. It never mutates its argument and depends on nothing but its inputs,
which is what makes story 28 hold: identical decision sequences reproduce identical
state because there is nothing else for the result to depend on.

**The encounter is not simulated here.** Its outcome arrives as `CompanionReturned` —
the module consumes results and never produces them (story 15). Whether a man comes
home is decided by play, which is what makes
[ADR 0001](../docs/adr/0001-tactical-agency-strategic-futility.md)'s promise true.
[ADR 0015](../docs/adr/0015-the-encounter-earns-survival-it-does-not-roll-it.md) settles
how the encounter itself earns that bool once it exists; nothing about the seam changed
as a result, which is why this module didn't either.

## Rules enforced by absence

Several stories are guaranteed by there being no API to break them, rather than by a
check someone has to remember:

| Story | Guarantee |
|---|---|
| 11 — the ride is capped by the bike | `BipOut.Companion` is one field, not a collection. A second passenger cannot be expressed. |
| 13 — nothing grows the set | The package exports no function that appends to `TheSet`. |
| 14 — no combat statistics | Every field on `Member` is a string. With nothing numeric, two members cannot be ordered. |
| 24 — a death is never a fail state | `EndReason` has no failure value, and losing a man returns no error. |
| 27 — one save slot | `Save` and `Load` take a directory and nothing else. There is no slot argument to pass. |

## Where this follows a later ADR over #3's wording

#3 was written before ADRs 0006–0013 landed. Where they disagree, the ADR wins and the
story's intent is preserved.

**Story 2 — "a finite budget per interval."**
[ADR 0012](../docs/adr/0012-scarcity-is-the-block-emptying.md) replaced the budget model
with availability: the player never spends anything, other people leave. A chapter opens
with everyone living outside; each evening sends `DepartAlongside` others in; the chapter
ends when nobody is left and going is what remains. The bound still exists — it is just
not a quantity the player holds.

**Stories 16–18 against ADR 0013.**
Stories 16 and 17 require every bip to escalate regardless of outcome.
[ADR 0013](../docs/adr/0013-escalation-plateaus.md) requires the intensity the player
meets to plateau. One counter cannot satisfy both, so there are two: `Cycle` advances on
every bip and nothing reduces it; `Intensity` is derived from it and stops at
`IntensityCeiling`.

**Joshua starts known.** Not in #3. He is Jamal's younger brother and lives with him
([cast.md](../docs/cast.md)), and you cannot not know your own brother. Not the special
case the cast contract forbids — the contract governs which *properties* a member
carries, and a starting value is not a property.

**The cast is present as data.** #3 put cast identity out of scope because it did not
exist yet. It does now ([issue #4](https://github.com/supersnoofer25-cyber/O-block/issues/4)),
so `cast.go` is [cast.md](../docs/cast.md) expressed as Go.

**Story 4 holds arithmetically**, not aspirationally. A chapter yields at most
`ceil(8/(1+DepartAlongside))` = 2 evenings, so a campaign yields at most 32, while
knowing all eight well would take `8 * EveningsToClose` = 40. Reaching high familiarity
with the whole set is impossible before anyone has even died.

## Deliberately not here

- **Flares.** [ADR 0011](../docs/adr/0011-the-last-bip-generates-the-next-flare.md) says
  the previous bip generates the next one, and `Cycle` is what a flare keys off — but a
  flare is authored content and an event on the Block, not campaign state.
- **Tie ripples.** `Member.Tie` carries the graph; what changes in a tied member when
  someone dies is writing, not state.
- **Cost-of-loss effects.** `Member.CostOfLoss` says what the Block loses. Making the
  Block lose it is engine work.
- **Encounter feel, bike handling, hub traversal, companion AI.** #3's `[playtest]`
  stories. Judged by playing, and automating them would bolt tests to an implementation
  that is expected to change.

## Tuning

`Chapters`, `IntensityCeiling`, `DepartAlongside` and `EveningsToClose` are gathered at
the top of [state.go](state.go). They are tuning rather than decision — `spec.md` open
question 2 is explicit that whether the encounters as built hit their target can only be
answered by playing them.

At the current values a campaign losing roughly one companion in four finishes chapter
sixteen with four of eight alive, which is the spread
[ADR 0007](../docs/adr/0007-zero-is-an-ending-not-a-fail-state.md) needs the ending to
handle.
