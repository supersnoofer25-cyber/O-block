# 0011. The last bip generates the next flare, and a flare kills people who are not in the set

- **Status:** Proposed
- **Date:** 2026-07-25

## Context

`spec.md` §3 has always contained the step *beef flares — a reason to go*, with nothing
behind it. Two later decisions changed what that step has to do.

[ADR 0008](0008-flat-chapter-sequence.md) settled that the player chooses *when* to bip out
and not *whether*, so a flare is not a gate on going. It is only the content of why —
which makes it a writing problem rather than a systems one, and raises the question of
whether it needs a system at all.

[ADR 0010](0010-the-beef-has-no-agreed-origin.md) removed the origin, so a flare cannot
reference one. Whatever generates it has to be self-sustaining.

**Where flares come from.** An external generator — a schedule, an escalation manager, a
character who brings you jobs — was the conventional answer and was rejected twice over. It
is a system doing what the fiction already does on its own, and it makes the **beef**
something managed rather than self-feeding, when **the cycle** is defined precisely as the
pattern *no participant controls*. It also tends toward a quest-giver, and
[ADR 0005](0005-scarce-time-outside.md) forbids quest markers and objective lists.

A purely ambient answer — the beef is a standing condition, so no particular event is
needed — is faithful to how `CONTEXT.md` defines **beef**, but leaves *a reason to go* with
no content. The player needs to know why tonight.

**What a flare does.** Near misses were the softer option: shots on the block, windows,
people indoors for a week, nobody killed. It keeps every death in the game a roster death,
which protects the weight of the eight. It was rejected because it produces a beef in which
the only people ever hurt are the eight men who chose to fight, and that is a milder world
than the one this game draws from. It also leaves the Block as scenery rather than a place
with anything at stake.

## Decision

We will generate each flare from the player's previous **bip**, and we will let flares kill
people on the Block who are not in the **set**.

**The previous bip is the whole mechanism.** Lose someone and you go for him. Hurt them and
get out clean and they answer it. Do very little and they still know it was you. In every
case the cause of tonight is what the player did last time, so no escalation system is
required. The first flare needs no special case either: the beef is inherited, so chapter
one's reason is simply the most recent thing that happened before the game began.

**Every flare is specific, recent and attributable** — the exact inverse of ADR 0010's
origin. The player always knows precisely why they are going tonight and never knows why
any of it started. That asymmetry is the cycle.

**A flare reaches the player as news, outside, from people.** Never a marker, an objective,
or a briefing.

**A flare never kills a member of the set.** They die on bips, where the player chose to put
them. A member killed outside would undercut the roster stakes
[ADR 0002](0002-permanent-death-shrinking-set.md) exists to create.

**A flare never touches anything that is part of a cost of loss** — not the eight, not
their people, not their places. Marcus's cars, James's church, Omar's hoop, Alex's sister,
Daquavion's girl and child are all reserved. Those eight subtractions belong to the eight
deaths and must stay attributable to them, which
[ADR 0009](0009-behaviour-and-absence-never-a-number.md) requires and
[ADR 0007](0007-zero-is-an-ending-not-a-fail-state.md) depends on for its ending.

**A flare does not kill every time.** When it does, the dead is someone the player could
have seen outside — named, placed, recognisable. Anonymous casualties every chapter would
make death cheap and drain the roster deaths of the weight everything else is built to give
them, which is ADR 0001's *anonymous waves dilute individual survival* relocated from the
**encounter** to the Block.

**It reads as aggression, not as consequence.** The causal chain from the player's bip to
their retaliation is fully visible and nothing about it is concealed. From inside the
fiction it still reads as *they hit us again*, so the player draws the wrong conclusion
from complete information — which is
[ADR 0001](0001-tactical-agency-strategic-futility.md)'s claim exactly: never deceived
about whether their skill works, only wrong about what it accomplishes.

## Consequences

**Easier.** There is no escalation system to build, tune, or explain, and no quest-giver to
write. The opening needs no special case. Each flare is a per-chapter event that doubles as
free characterisation — who reacts how, who wants to go, who has heard a different version
— which matters under [ADR 0003](0003-choice-lives-in-systems-not-dialogue.md), where such
instruments are scarce. And ADR 0001's thesis is enacted every chapter without ever being
stated: the player goes out to protect the Block, and the direct result is the Block being
hurt.

**Harder.** Every chapter needs an authored flare, and they must intensify across a campaign
whose escalation curve is deliberately still undecided. Writing them is now coupled to
tuning that does not exist yet.

The real difficulty is tonal, and it is the hardest writing in the project after the ending.
Deaths of people outside the set must never become spectacle, a body count, or a device
that exists to motivate the player. The moment a flare death reads as fuel, the Block
becomes a place where anonymous people die so that the protagonist has somewhere to go —
which is both the failure named above and a worse thing to have made.

**Foreclosed.** Any external flare generator, escalation manager, or quest-giver. Flares
that kill members of the set. Flares that damage or remove any part of a cost of loss.
Anonymous casualties. Any presentation of the causal chain as instruction to the player.

**Worth revisiting if.** Playtesting shows players reading flares as the game telling them
what to do rather than as something that happened. That would mean the news framing has
drifted toward briefing, and the fix is in how it reaches the player — not in adding a
system to deliver it more clearly.
