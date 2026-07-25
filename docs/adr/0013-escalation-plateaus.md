# 0013. Escalation climbs early and then plateaus; the campaign does not get harder toward the end

- **Status:** Proposed
- **Date:** 2026-07-25

## Context

[ADR 0001](0001-tactical-agency-strategic-futility.md) fixes that escalation is monotonic
and that winning a **bip** still escalates. [ADR 0008](0008-flat-chapter-sequence.md) gave
that climb a domain — sixteen chapters — and both deliberately declined to fix its shape.
With everything else settled, the shape is now the last thing the campaign's contents are
authored against, and flare material written for a rising curve does not work under a flat
one.

**The conventional answer is a curve that accelerates**: quiet early, coming apart in the
last third. It is what almost every campaign does, and it is the wrong shape here for two
reasons.

ADR 0008 forecloses a mid-campaign structural turn on the grounds that such a turn *is*
ADR 0001's corrective feedback delivered early. An accelerating intensity curve produces
the same thing without the structure — a felt inflection, a *this is when it got bad*,
which tells the player the story is building toward a resolution. There is no resolution to
build toward.

The second reason is sharper and was not obvious. A rising curve makes the player's own
losses **illegible**. Late in a campaign the player is down three or four men and things are
harder. Under a rising curve they cannot tell whether it is harder because of who they lost
or because the game turned the dial, and they will reasonably assume the dial. That hands
them something other than their own choices to attribute the outcome to, which is precisely
what ADR 0001 cannot afford.

**A linear climb** is the safe alternative and has a weaker version of the same problem,
plus nothing to recommend it beyond being unobjectionable.

## Decision

We will have escalation climb through roughly the first third of the campaign and then
plateau at its ceiling for the rest.

Concretely, against ADR 0008's sixteen chapters: it rises over about the first five or six,
and the remaining ten sit at the top. The climb exists so the player feels the direction
once and learns which way this goes. After that the direction is established and needs no
further demonstration.

**The plateau is the statement.** It says *this is how it is now* — which is what
`CONTEXT.md` already defines a **beef** to be, a condition that persists, and what it
defines **the cycle** to be, something that outlives the people in it. There is no
crescendo, no darkest hour, and no worst night. Chapter sixteen is no more intense than
chapter eight. It is bad, it stays bad, and it will still be bad after the game ends.

This remains monotonic as ADR 0001 requires — a plateau is non-decreasing.

**What the player experiences still worsens**, because two other things keep moving under a
constant threat: the **set** shrinks, and under
[ADR 0012](0012-scarcity-is-the-block-emptying.md) the Block empties, so later chapters are
shorter and have less in them. Holding the threat constant is what makes those legible. When
the back half is harder, it is harder because there are fewer of you — and with the dial
still, that is the only available reading.

## Consequences

**Easier.** The player can only attribute a worsening campaign to their own losses, which is
ADR 0001's individual agency protected at the one scale where a difficulty curve would have
quietly undermined it. Flare and encounter material for the back ten chapters is authored
against a fixed intensity rather than a moving target, which is less tuning and less rework.
And the ending is not a climax, which
[ADR 0007](0007-zero-is-an-ending-not-a-fail-state.md) needs — it is where the argument
becomes legible, not where the action peaks, and a curve that peaked into it would frame it
as a final battle.

**Harder.** This compounds ADR 0008's named monotony risk rather than relieving it. Flat
chapters at a flat intensity for ten chapters is a real hazard, and we have now declined the
two conventional remedies for it in a row. Everything rests on the roster and the Block
changing enough underneath to carry it — which ADR 0002 promises makes playthroughs diverge
structurally, and which ADR 0012 makes visible chapter to chapter. If that is not enough,
this is the second decision that breaks.

There is also no set-piece available at the point where an audience expects the biggest one,
and its absence will be read by some as the campaign running out of ideas rather than
declining to build to something.

**Foreclosed.** An accelerating curve, a final-act intensity spike, a darkest hour, and any
chapter authored as the worst one. Difficulty that rises with chapter number.

**Worth revisiting if.** Playtesting shows the back half reads as flat rather than as
relentless. The fix is in what fills those chapters — the roster and the Block are supposed
to be carrying them — not in raising the curve, which would trade a pacing problem for the
attribution problem this decision exists to prevent.
