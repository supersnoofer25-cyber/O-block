# 0005. Time outside is scarce, and knowing the set is missable

- **Status:** Accepted
- **Date:** 2026-07-24

## Context

[ADR 0003](0003-choice-lives-in-systems-not-dialogue.md) removed the dialogue choice
system, which is the instrument narrative games normally use to build attachment to a
cast. [ADR 0002](0002-permanent-death-shrinking-set.md) simultaneously requires that
losing a member of the **set** hurts — which only happens if the player knows who they
lost.

That left a single place for characterization to live: **outside**, the time between
bips. This ADR decides its shape, and it is not a pacing detail — it is the entire
attachment architecture, and everything ADR 0002 promises depends on it.

The alternative was an open hub with unlimited time, where the player can meet all eight
before every bip. It is warmer, carries no stress, and has no missable content. It was
rejected because when all eight are known equally the roster choice flattens into a
comparison of capabilities, and nothing about who the player chose to care about is ever
revealed — which is precisely the signal the design needs.

## Decision

We will make the Block a small, dense hub traversed on foot, and the player's time
outside scarce.

On foot deliberately: **the bike** is only for **bipping out**, so walking reads as a
different mode entirely and the contrast carries the tonal shift without needing to be
stated. The hub is small and always populated — a handful of locations, learned the way a
resident learns them, with no quest markers and no objective list. Set members are
somewhere in it doing something, and the player finds them by knowing where they tend to
be.

Time outside is limited, so the player cannot come to know all eight. Who they spend that
time with is therefore the characterization mechanic. The player will know three or four
well and the rest barely — and the ones they know are the ones they cannot afford to
lose, which is the tension the whole design has been building toward. The player authors
their own tragedy rather than receiving it, and it costs nothing in branching dialogue.

## Consequences

**Easier.** Attachment is produced by the player's own choices rather than asserted by
writing, so it is proportionally stronger. The roster decision gains its final layer: the
person you know best is both the one you most want on the back and the one whose loss
costs most, and that tension falls out of the structure for free. Characterization
content is authored linearly per character, with no branch tree.

**Harder.** The scarcity has to feel like a fact of life rather than a timer; if it reads
as an artificial stress mechanic, players will resent it and optimize against it instead
of choosing honestly. The hub must be dense and legible enough that finding people is
knowledge rather than searching.

**Accepted deliberately.** Character content is missable, and players who never spent
time with someone will not care when that person dies. This is the system working, not
failing. Some deaths landing flat is the necessary cost of others landing hard, and we
are not to "fix" it by ensuring every character gets a guaranteed introduction — doing so
would restore exactly the flat, equal familiarity this decision exists to prevent.

**Worth revisiting if.** Players report the time limit as stressful rather than
meaningful. The correct response is to adjust how the scarcity is *expressed* — pressure
from the fiction rather than a visible clock — not to remove the limit.
