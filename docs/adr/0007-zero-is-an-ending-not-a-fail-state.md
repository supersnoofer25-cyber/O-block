# 0007. The campaign has one ending; an empty set reaches it early, and never as a failure

- **Status:** Proposed
- **Date:** 2026-07-24

## Context

[ADR 0002](0002-permanent-death-shrinking-set.md) commits to a **set** of eight that only
ever shrinks, with no recruitment, no replacement, and no exemptions. It follows that the
set can reach zero, and nothing yet said what happens when it does. The campaign-state
spec flagged this as the one question the module cannot be finished without, because every
other rule is written for "any subset alive" and the empty subset is the case that phrase
does not cover.

The loop cannot simply continue. [ADR 0004](0004-dirt-bikes-only-and-per-bip-seat-choice.md)
requires the player to choose a seat and choose who fills the other one before every
**bip**. At zero there is nobody to fill it. Of the three systemic decisions the design
runs on, two — who to spend time **outside** with, and who to bring — have no options
left. The choice architecture is empty, not merely constrained.

Three options were genuinely on the table.

**Protect the last member.** The set bottoms out at one and the campaign proceeds. Cleanly
solves the mechanical problem and is rejected immediately: it is plot armour, which
[ADR 0002](0002-permanent-death-shrinking-set.md) forecloses by name and which players
detect and resent — retroactively cheapening every death that was real.

**Let the player bip out alone.** A bike carries up to two, so riding solo is physically
available. Rejected because it is a different game: the player rides and shoots, or rides
and does not shoot, and either way the seat decision that
[ADR 0004](0004-dirt-bikes-only-and-per-bip-seat-choice.md) built the entire preparation
layer around no longer exists. It also removes the only thing at stake on a bip, since
there is no longer anyone who might not come home.

**Zero ends the campaign.** Structurally forced by the above, and therefore the decision is
really about what kind of ending it is, and whether it is the only one.

That second half matters more than it looks. If reaching zero were the destination — if
the campaign simply ran until everyone was dead — the design would be
[ADR 0001](0001-tactical-agency-strategic-futility.md)'s explicitly rejected *pure
tragedy*, in which player skill changes nothing and players correctly perceive the outcome
as rigged. ADR 0001 rejected that option on purpose. An empty set must be reachable
without being inevitable.

## Decision

We will give the campaign a single ending, fixed by its chapter structure and not by the
roster. How many of the set are alive when it arrives is the variable the player's choices
produce.

An empty set reaches that same ending early. When the last member dies, the campaign does
not continue with a hollowed-out loop and does not stop — it proceeds to its ending from
wherever it is. Reaching zero is therefore a possible state at the ending rather than the
purpose of the campaign, and a player can finish with any number from eight down to none.

**It is never a failure.** No "mission failed", no game over, no reload prompt, no
retry — [ADR 0002](0002-permanent-death-shrinking-set.md) binds here as it does everywhere
else, and this is the last and most tempting place to break it. Zero is instead the first
moment the game's argument is legible: the player performed well, every time, and the set
is gone anyway. ADR 0001 says the player is never deceived about whether their skill works,
only about what it accomplishes, and that they keep winning so they never receive the
feedback that would correct them. This is that feedback, arriving at the only point where
it can no longer be misread.

**Nothing at the ending tallies the survivors.** No count, no score, no percentage, no
list of who made it. A survival tally would convert every death in the campaign into a
score in the final seconds — precisely the fail-state framing ADR 0002 forbids, arriving
at the last possible moment and retroactively reframing everything before it.

**The ending is populated by what is missing.** Under the cast contract each loss subtracts
something from the Block, persistently. The Block at the ending is therefore exactly as
empty as the player made it, with no additional system required. At zero it is at its
emptiest, and that is the whole statement.

## Consequences

**Easier.** The module gains its missing boundary case, and "playable with any subset
alive" is now total rather than almost-total. The campaign has a defined length
independent of the roster, so writing can target a known end point. Every playthrough
converges on one authored ending, which keeps the hardest writing in the project — the
payoff ADR 0001 calls its most difficult — a single scene to get right rather than a
branch set.

**Harder.** That ending must work across a nine-way spread of surviving rosters, from
eight alive to none, and it has to land without a lecture in all of them. The empty-set
version is the hardest of the nine: it has no one left to carry a scene, so it has to be
carried by absence alone. Early termination also has to feel like arrival rather than
truncation, or players will read it as the fail state this decision spent its whole
argument avoiding.

**Foreclosed.** Any last-member protection. Bipping out alone. Multiple endings selected
by roster count, and any ranking of outcomes as better or worse. Survivor counts, scores,
and completion statistics anywhere in the ending.

**Deliberately left open.** Whether **Jamal** himself survives. That is ending-writing
rather than architecture, it does not block the module, and deciding it here would
constrain the hardest scene in the project before anyone has attempted it.

**Worth revisiting if.** Playtesting shows players reaching zero read it as having lost
despite the framing. The response is to strengthen how the ending expresses arrival —
not to prevent zero, which would reintroduce plot armour and undo
[ADR 0002](0002-permanent-death-shrinking-set.md).
