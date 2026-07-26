# 0015. The encounter earns a companion's survival by threat denied, not by a roll

- **Status:** Accepted
- **Date:** 2026-07-25

## Context

`campaign.BipOut` takes `CompanionReturned bool` and consumes it without ever deciding it
(story 15) — correct, because the module is engine-agnostic and has no combat to simulate.
`cmd/roundhill` produces that bool with `rng.Intn(loss)`, which is a text harness doing the
only thing available to it. The real game cannot inherit that. [ADR
0001](0001-tactical-agency-strategic-futility.md) promises the player a one-step causal
line — *you covered the stairwell, so Marcus made it home* — and `spec.md` §6.3 is explicit
that this is "not a probability the game rolls." Nothing built so far has tested that
promise, because nothing built so far has an encounter.

Three things already decided elsewhere box in the answer tightly before this ADR adds
anything. [ADR 0006](0006-familiarity-changes-legibility-not-competence.md) fixes a
companion's competence regardless of familiarity — whatever decides survival cannot vary
per member or per relationship. [ADR 0009](0009-behaviour-and-absence-never-a-number.md)
forecloses any stat, rating, or threshold that differs by member. And CLAUDE.md's blanket
rule forecloses surfacing any number to the player at all, which rules out a health bar or
danger meter as the feedback channel.

Two shapes were on the table for what actually happens during the seconds of gunfire.

**An authored save-beat per encounter** — a specific scripted moment, a chokepoint or a
timed action, that is the actual check. It gives a writer direct control over the causal
line, but it multiplies with content: eight members across twelve chapters in either seat
is up to 192 pairings, and [ADR 0014](0014-unreal-5-on-pc.md) already named studio-sized
content arriving at a solo developer as the project's real risk. Authoring a save-beat per
pairing is exactly that risk, applied to the one system this ADR is trying to make
affordable.

**A systemic threat model**, reused identically across every bip. One mechanism, tuned
once, that produces a different outcome each time because the player and the encounter
geometry differ, not because the rule does.

## Decision

We will give the companion a hidden, per-bip tally of unaddressed danger, and resolve
`CompanionReturned` from whether it crosses a single fixed threshold — the same threshold
for all eight, because a threshold that varied by member would be exactly the stat ADR 0009
forecloses. Nothing about the tally persists between bips; each encounter starts clean, the
same way `Apply` already treats every `BipOut` as self-contained.

**What "denying a threat" means depends on which seat the companion is in, mirroring the
split ADR 0004 already made between the seats:**

- **Companion riding, player on the back.** The player is the one shooting. A threat that
  would have hit the rider and is killed or suppressed before it fires adds nothing to the
  tally. Miss the window and it does. This is the literal case the causal line describes:
  the gun the player put down before it came up.
- **Companion on the back, player riding.** The companion is the one shooting, and his aim
  is fixed and always competent (ADR 0006) — it is never the variable here. What the player
  controls instead is exposure: how long the pair sits somewhere with an open sightline,
  which route is taken, when to leave. Denying a threat in this seat means never giving it
  the angle it needed, not killing it.

Both collapse to the same currency — threats that never connect — reached by the verb each
seat actually gives the player, which is what ADR 0004 already promised: riding owns the
approach and the escape, the back owns the violence.

**Resolution is never a roll.** Whether a shot connects is decided by the same skill-driven
mechanics that resolve combat aimed at the player — aim, position, timing — never a
probability check run on the companion's behalf. This is `spec.md` §6.3's rule extended to
the one place it hadn't yet reached.

**The tally and its threshold are simulation state, never presentation.** No companion
health bar, no danger meter, no number anywhere a player can read progress off it. The only
channel is the one ADR 0006 already built for exactly this problem: animation, audio, and
what the companion says unprompted.

**Familiarity plugs in here, and only here.** How much warning the player gets that a
threat exists is gated by familiarity, not the threat itself. A companion Jamal knows well
telegraphs early and legibly — a call-out before the angle opens, a look, a hand signal a
stranger wouldn't give. A companion he doesn't know gives the same information late,
ambiguous, or not narrated at all. The tally, the threshold, and enemy behaviour are
identical either way; only the player's actionable warning time changes. That is exactly
ADR 0006's line that the effect "routes through the player's skill rather than around it" —
familiarity buys information, and converting information into a save is still the player's
job.

**`campaign.BipOut` does not change.** `flare.go`'s `news()` already branches only on
`LastBip.Lost` — there is no consumer anywhere that needs more than the bool it already
gets, and inventing one now would be scope this project does not need yet. The tally lives
and dies entirely inside the encounter; only its verdict crosses the seam, which is story
15 unchanged: `Apply` consumes a result, it does not produce one.

**`cmd/roundhill`'s dice roll is unaffected and is not an implementation of this ADR.** It
remains what it always was — a stand-in a text harness uses because it has no combat to
simulate. Nobody should read its output as a preview of how the shipped game decides this.

## Consequences

**Easier.** One mechanism serves every companion, every seat, every chapter, so the content
load this ADR exists to avoid never arrives. The causal line is emergent from actual play
rather than authored per pairing, which means it is also automatically consistent — a
player who covers the same angle twice gets the same kind of result twice, without anyone
having scripted either encounter to guarantee it. ADR 0006's behavioural-expression work
gets a second job it was always going to need eventually (communicating danger, not just
communicating who someone is) instead of a second system built to do it.

**Harder.** The tally has to be legible without ever being shown, which means the
companion's reactions — flinching, ducking, a shout — carry real informational weight and
not just flavour. That is real animation and audio scope stacked on top of what ADR 0006
and ADR 0009 already asked for, and it is again the kind of scope that gets cut first
because nothing visibly breaks when it's thin. Tuning one threshold that has to feel right
across eight different capabilities and two different seats is harder to validate than
tuning eight things separately, precisely because there is only the one number and it has
to work everywhere at once.

**Foreclosed.** Any survivability stat, threshold, or health value that differs by member —
that is the ADR 0009 stat by another name. Any UI element that shows the tally, its
threshold, or how close a companion came. Scripted save-beats as the primary mechanism for
deciding the bool (a chapter may still stage a pivotal encounter dramatically, but the
systemic tally is what actually decides the outcome, not a hand-placed trigger). Any
resolution of a hit or a miss by chance rather than by the same mechanics that resolve fire
against the player.

**Worth revisiting if.** Playtesting shows players cannot tell what specifically saved or
doomed a companion — the one-step causal line not landing. The fix, following the pattern
ADR 0006 already set for exactly this failure mode, is to strengthen the behavioural and
audio expression of danger and warning, not to add a meter or bring back a roll. Separately,
if the riding seat's exposure-based denial reads as arbitrary rather than as skill, the fix
is to make route and timing consequences more legible in level geometry — sightlines,
timing windows a player can learn to read — not to add a number that would tell the player
directly.
