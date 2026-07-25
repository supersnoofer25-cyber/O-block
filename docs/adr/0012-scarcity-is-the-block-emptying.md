# 0012. Scarcity is the Block emptying, not a budget the player spends

- **Status:** Accepted
- **Date:** 2026-07-25

## Context

[ADR 0005](0005-scarce-time-outside.md) made time **outside** scarce so that who **Jamal**
spends it with becomes the characterization mechanic, and attached a requirement it did not
say how to meet: the scarcity *must read as a fact of life rather than a timer*, because a
player who perceives a stress mechanic will optimise against it instead of choosing
honestly.

[ADR 0008](0008-flat-chapter-sequence.md) settled the unit — one chapter is one budget of
time outside plus the **bip** that ends it — and settled that the budget running out, rather
than a clock, is what sends the player. It deliberately left open what the budget is made
of. That is the last thing standing between the design and its own central mechanic.

The difficulty is narrow and specific. The moment a player can *count* what remains, they
stop choosing who they want to see and start allocating — *two left, and Marcus is useful,
and I have not done Daquavion yet*. That is the precise inversion this mode exists to
prevent: who the player spends time with is supposed to reveal who they care about, and a
countable budget makes it reveal who they intend to use.

**Visits or stops** — an explicit allowance per chapter — is the obvious implementation and
the worst. It is a number on screen, and nothing else needs to be said about it.

**Days** — Tuesday, then Wednesday, then you go. Countable as soon as the player learns how
many there are, and it drags in a great deal of fiction the game does not otherwise need,
starting with what happens at night.

**Light** — one chapter is one afternoon, and you go when it is dark. No units, which is
better, but the rhythm is learnable within an hour of play, and a sun moving across the sky
is a progress bar with extra steps.

All three share a shape: the player holds a quantity and spends it. That shape is the
problem, not the units it is denominated in.

## Decision

We will express scarcity as **availability rather than quantity**. The player never spends
anything. Other people leave.

Everyone living is outside when the chapter opens. Spending real time with one member means
the others carry on without you — and people go in. You were with Marcus, so you were not
there when Omar went upstairs. Nothing was deducted; you were simply somewhere else.

**The chapter ends when there is nobody left outside.** Not when a meter empties. The Block
goes quiet, the player is standing on it alone, and going is the only thing left to do —
which is [ADR 0008](0008-flat-chapter-sequence.md)'s *the things to do outside simply run
out* stated as a place rather than as a rule.

**Nothing is ever counted or surfaced.** No allowance, no clock, no meter, no day count, no
"time remaining", and no notification that someone has gone. The player sees who is still
out because they can see the Block, which is what a resident would know and is not a
readout.

**Departures are deterministic but unannounced.** Determinism is required — [issue
#3](https://github.com/supersnoofer25-cyber/O-block/issues/3)'s story 28 needs identical
decision sequences to reproduce identical state, or autosave-only checkpointing stops
guaranteeing anything. Determinism is not the same as legibility: a first-time player has
no way to know the pattern, and [ADR 0002](0002-permanent-death-shrinking-set.md)'s
autosave-only rule denies them the repetition that would teach it.

**The opening is the exception.** During the outside-only opening nobody leaves, because
ADR 0002 requires the player to meet all eight within the first hour. It is the one time
the Block is full, and it is the picture every later chapter erodes.

## Consequences

**Easier.** There is no number anywhere, so there is nothing to optimise against — ADR
0005's requirement is met structurally rather than by dressing a counter in fiction. The
signal it wanted comes out in its purest form: under uncertainty about who will still be
out, who the player goes to *first* is who they care about, and no other reading is
available.

The chapter's ending carries the game's argument without stating it. The bip happens when
there is nobody left to be with.

And the campaign compresses on its own. As the **set** shrinks there are fewer people to
spend an evening with, so later chapters are shorter and emptier with no authored pacing
curve. That is not a contradiction of ADR 0008's flat sequence — the *shape* of every
chapter is identical, and only its duration changes, because there is less of the Block
left to be in.

**Harder.** A player who misses someone may experience it as the game taking something
away. It has to read as evening rather than as a rule, and there is no safe way to explain
it without reintroducing the mechanism as a mechanism.

Departures must be legible enough to feel like people going home and opaque enough never to
become a schedule. That is a fine line and it is where this decision will fail if it fails.

Each member also needs somewhere to be and a plausible reason to go in, per chapter, which
is real ambient authoring on top of what [ADR 0009](0009-behaviour-and-absence-never-a-number.md)
already asks of them.

**Foreclosed.** Counters, meters, clocks, day counts, visit or stop allowances, any
surfaced "time remaining", and any notification that a member has left. Randomised
departures, which would break story 28.

**Worth revisiting if.** Players report feeling robbed rather than rushed. That would mean
departures are reading as a rule being enforced rather than as the Block going quiet, and
the fix is in how they are staged — not in restoring a budget, which would hand back the
countable quantity this decision exists to remove.
