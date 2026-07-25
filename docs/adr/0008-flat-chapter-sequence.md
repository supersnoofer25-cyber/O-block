# 0008. The campaign is a flat, fixed sequence of chapters, and a chapter is one budget of time outside and the bip that ends it

- **Status:** Accepted
- **Date:** 2026-07-24

## Context

[ADR 0007](0007-zero-is-an-ending-not-a-fail-state.md) tied the campaign's ending to its
chapter structure rather than to the roster, which made a previously idle question
load-bearing: the campaign cannot have a defined end until it has a defined shape.

Two questions were tangled together and turn out to be one. The design already needed an
*interval* — [ADR 0005](0005-scarce-time-outside.md) makes time **outside** scarce, and
scarcity requires a unit over which the budget is spent. It separately needed a *chapter*.
Nothing said these were the same thing, and treating them as two concepts would have meant
authoring two structures over the same loop.

**What a chapter is.** The alternative was the conventional one: a chapter as an authored
story beat of variable length, containing however many **bips** the beat requires. It was
rejected because it puts pacing in the writing rather than in the systems, and this design
has consistently put weight in the systems layer
([ADR 0003](0003-choice-lives-in-systems-not-dialogue.md)). It also leaves the scarcity
budget without a natural boundary, which is the thing ADR 0005 most needs and least
specifies.

**How many.** This is constrained from both sides, and the constraint is tighter than it
looks. A bike carries two ([ADR 0004](0004-dirt-bikes-only-and-per-bip-seat-choice.md)), so
at most one member of the **set** can be lost per bip. Reaching zero therefore requires at
least as many bips as there are people — a campaign shorter than the roster makes ADR
0007's empty set unreachable, and the case it spent its whole argument on becomes
theoretical. But a long campaign is worse: as length grows, cumulative losses approach
certainty, most players terminate early, and zero stops being a reachable outcome and
becomes the destination. That is
[ADR 0001](0001-tactical-agency-strategic-futility.md)'s explicitly rejected pure tragedy,
arriving through pacing rather than through intent.

**What shape.** A three-act structure with a mid-campaign turn was on the table and is what
a narrative game of this kind would normally do. It is incompatible with ADR 0001, which is
explicit that the player keeps winning and therefore *never receives the feedback that would
correct them*, and with ADR 0007, which makes the ending the first point at which the
argument is legible. A structural turn in the middle is that feedback, delivered early. The
campaign cannot have one.

## Decision

We will make a chapter one fixed budget of time outside plus the single bip that ends it,
and the campaign a fixed sequence of such chapters that are all structurally identical.

**A chapter ends when the player takes the bip.** The player chooses when, not whether.
There is no countdown and no forced departure: the budget of things to do outside simply
runs out, and going is what is left. This is how ADR 0005's requirement that scarcity read
as a fact of life rather than as a timer is met — nothing expires, the player has just
spent what they had.

**Chapters are an authoring structure and are never surfaced as a count.** No chapter
number, no "7 of 16", no progress bar. A visible position in a fixed sequence is a timer
wearing different clothes, and it invites exactly the optimisation ADR 0005 exists to
prevent.

**The sequence is flat.** Every chapter has the same shape. There are no acts, no
mid-campaign turn, and no structural escalation of form. What changes across the campaign
is who is alive, who the player knows, and how far **the cycle** has escalated — never the
shape of the thing. The repetition is the argument: **the cycle** is defined as a pattern
that feeds itself, and a campaign that dramatises it by changing shape is describing it
instead of enacting it.

**Length is set so that zero is reachable and unlikely.** The tuning-independent rule,
which holds whatever the numbers become:

- Chapter count must exceed the roster size, or an empty set is unreachable and ADR 0007's
  central case is fiction.
- Expected losses across a full campaign should land near half the roster, so that the
  common outcome is a diminished set rather than an empty one, and both extremes — nobody
  lost, everybody lost — are rare tails rather than likely results.

As a starting point and explicitly as tuning, not decision: **sixteen chapters** against a
roster of eight, with roughly one bip in four costing someone. That yields about four
survivors typically, the full nine-way spread ADR 0007 requires the ending to handle, and
zero as a bad run rather than an expectation.

**The opening is outside only.** It ends when the first bip is taken. ADR 0002 requires the
player to meet all eight within the first hour, and the set cannot start shrinking before
the player has met it. This is structure rather than protection — there is no bip yet on
which anyone could be lost, and no member is exempt once there is.

## Consequences

**Easier.** The scarcity budget gains the boundary ADR 0005 needed and never specified, and
the interval and the chapter stop being two things. The escalation curve, which the
campaign-state spec deliberately left thin, now has a defined domain to be drawn across.
The ending has a known arrival point, so the hardest writing in the project can be aimed at
something. And the player can no longer decline a bip indefinitely, without ever being
pushed by a clock.

**Harder.** Sixteen structurally identical chapters is a real monotony risk, and it is one
we have deliberately declined the usual remedy for. The variation has to come entirely from
who is alive and who the player knows — which differs per player and which
[ADR 0002](0002-permanent-death-shrinking-set.md) already promises makes playthroughs
diverge structurally rather than cosmetically. If that divergence is not strong enough to
carry the repetition on its own, this decision is what breaks first.

The combinatorial writing load also now has a number attached to it: sixteen chapters, each
of which must work against an arbitrary subset of eight. ADR 0002 named that load as the
project's principal scope risk and committed to shrinking the roster rather than adding
plot armour. There is now a cheaper lever to pull first — chapter count — and it should be
pulled before the roster one, which is the most destructive of the three available
(bounding ties, then chapters, then the roster itself).

**It revises one of the three systemic decisions.** `spec.md` §8 listed *whether* to bip
out among them, following [ADR 0003](0003-choice-lives-in-systems-not-dialogue.md)'s
framing that choosing to retaliate is choosing to make things worse. Under this decision it
is *when* to bip out, not whether.

That is a genuine narrowing, and it was tested against the alternative before being
accepted. Letting the player decline a chapter's bip outright produces a dominant strategy
either way: if declining avoids escalation, never going is optimal and the game has an
opt-out; if declining escalates anyway, it is strictly worse than going and no one would
choose it. Neither is a choice.

The narrowing is also the more honest reading of the fiction. **Beef** is defined as a
condition that persists, with no terms under which it could be declared over. A condition
is not something one declines. That the player cannot opt out is the point rather than a
concession, and it makes the remaining decisions — when, and with whom — carry the weight
the opted-out one was never going to.

**A sharp edge worth naming.** A player who reaches zero early gets a materially shorter
campaign. Under ADR 0007 that is arrival rather than truncation, but it is arrival that
costs the player content, and it will read as punishment if it happens often. That is the
strongest reason length and loss rate must be tuned together rather than independently.

**A full roster at the ending is not a win.** Escalation is monotonic under ADR 0001, so a
campaign finished with all eight alive is one where **the cycle** is at its worst and the
cost has simply not been collected yet. Nothing may present it as the good outcome — ADR
0007 already forecloses ranking endings, and this is the case where the temptation is
strongest.

**Foreclosed.** Acts, mid-campaign structural turns, and any beat that delivers the
design's argument before the ending. Variable-length chapters containing multiple bips.
Any surfaced chapter count, progress bar, or position indicator. Forcing a bip by
countdown.

**Worth revisiting if.** Playtesting shows the middle of the campaign sags. The response is
to cut chapters, not to add an act structure — the flatness is carrying an argument, and
the length is the part that is tuning.

## Tuning log

**2026-07-25 — sixteen chapters cut to twelve; loss rate from one bip in four to one in
three.** The decision is unchanged. Both numbers were offered above as a starting point
and explicitly as tuning, and this is that tuning happening.

The revisit condition fired, but not on the first reading. `cmd/roundhill` played through
as flat the first time, and that reading was discarded as worthless: the harness had one
fixed `Activity` line per member and no flare at all, so the roster and the Block — the
two things this decision leaves the variation entirely to — had nothing to vary with. The
monotony was the missing content, not the shape. It was the predicted symptom arriving for
the wrong reason, which is the failure mode this log exists to record, because the obvious
response would have been to cut the campaign on evidence that did not support it.

With [ADR 0012](0012-scarcity-is-the-block-emptying.md)'s per-chapter ambient content and
[ADR 0011](0011-the-last-bip-generates-the-next-flare.md)'s flares written, a full replay
still went flat from around the tenth chapter. That is the same reading with the confound
removed, and it is what twelve answers.

Twelve is the shortest cut that clears where it sagged and stays above this decision's own
floor — the count must exceed the roster of eight, or ADR 0007's empty set is unreachable.
The loss rate moved with it and not independently, as the sharp edge above requires: twelve
chapters at one in four would land a typical campaign three men down instead of four, and
the diminished set the ending is written for would have quietly become a fuller one. At one
in three, twenty auto-played campaigns averaged 4.4 losses against the target of half the
roster, with no run losing nobody and none losing all eight — both extremes still tails.

What has not been tested is whether twelve is itself too long. The sag was measured at
sixteen, and the next honest reading comes from playing twelve through.
