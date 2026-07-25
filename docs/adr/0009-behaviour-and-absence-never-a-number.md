# 0009. A member is one behaviour on a bip and one absence on the Block, and neither is a number

- **Status:** Proposed
- **Date:** 2026-07-25

## Context

[ADR 0003](0003-choice-lives-in-systems-not-dialogue.md) placed the game's entire choice
architecture on the question of who fills the other seat.
[ADR 0004](0004-dirt-bikes-only-and-per-bip-seat-choice.md) made the player's own seat a
separate per-**bip** decision. [ADR 0002](0002-permanent-death-shrinking-set.md) made the
answer permanent. All three depend on the eight members of the **set** being genuinely
different from one another, and none of them said what a member consists of.

`spec.md` §4 had a constraint — differentiated *by relationship and consequence, never by
combat class or loadout* — but a constraint is not a model, and the first attempt at
authoring the eight went straight past it. What arrived was competence ratings on the
game's two verbs: good at driving and not much of a shot, good at shooting and a bad
rider, good at dodging, carries the most ammunition. That is the natural way to describe
characters in an action game, and it is worth recording that it was not a careless
answer — it is what the form invites.

It fails twice over. Eight people scored on two axes is a comparison table, which is the
loadout screen ADR 0003 exists to prevent. And more sharply, it collapses two decisions
into one: if one man drives well and another shoots well, then choosing your seat chooses
your companion, and the preparation layer that ADR 0004 built has one decision in it
instead of two.

A second question followed once capabilities were behaviours. A member also carries a cost
when he is killed, and the cost has to come from somewhere. Drawing it from the same trait
as the capability — the man who talks constantly is missed because the Block goes quiet —
is the tempting move and reintroduces the same failure by the back door: the two halves
become one fact about a person, and one fact about a person is a value that can be ranked.

## Decision

We will describe a member of the set with exactly two things, drawn from disjoint facets of
him.

**One capability, expressed as a behaviour, defined in both seats.** Not a rating, not a
class, not a loadout. Marcus knows the way out — riding, he takes exits you would not have
found; on the back, he is telling you where to go before you asked. The same man is worth
bringing wherever the player sits, so the seat decision and the companion decision stay
independent. Every capability carries its own edge, so none is correct on every bip: Alex
does not hesitate, and sometimes committing is wrong.

**One cost of loss, drawn from what the man is outside.** A capability is exercised
**bipping out**; a cost is paid **outside**. The two must not be derivable from one
another. The shape is `spec.md` §4's own — *the one who never panics is the one whose
mother feeds me* — where the trait and the price have nothing to do with each other, so a
player optimising the roster cannot see the price coming.

**No two costs may subtract the same kind of thing**, or a player cannot tell whose absence
caused what.

**A cost hollows a place; it never removes one.** The church stands and is used less. The
house is still there and is empty. If costs deleted locations the hub would shrink as the
campaign ran, and [ADR 0005](0005-scarce-time-outside.md) needs it small, dense, and
legible enough that finding people is knowledge rather than searching.

## Consequences

**Easier.** The roster cannot be ranked, so ADR 0003's screen keeps its weight without
needing to be defended in the UI. There are no character balance passes, because there is
nothing to balance. Costs compose without being designed to: eight distinct subtractions
accumulate into a Block that has lost its food, its work, its mobility, its gathering
place, its play, its children and finally the player's own home — which is exactly what
[ADR 0007](0007-zero-is-an-ending-not-a-fail-state.md) said the ending would be made of, at
no additional cost.

**Harder.** Every capability has to be legible in play with no number and no interface
element to announce it, which puts the load on animation, audio, VO and companion AI —
the same scope [ADR 0006](0006-familiarity-changes-legibility-not-competence.md) already
leans on, and the same scope that gets cut first because nothing visibly breaks when it is
missing. Authoring a member is craft rather than tuning: a member who is not working cannot
be fixed by adjusting a value, only by rewriting who he is.

**Foreclosed.** Stats, ratings, tiers, levels, classes and loadouts on any member. Costs of
loss derived from capabilities. Costs that delete a location. Any capability that exists in
only one seat.

**Worth revisiting if.** Playtesting shows players cannot tell the eight apart in play, or
default to one companion regardless of the bip. The first means the behavioural expression
is too weak and should be strengthened; the second means some capability lacks its edge and
is simply correct too often. Neither is fixed by adding numbers — numbers are what would
make the defaulting rational rather than habitual.
