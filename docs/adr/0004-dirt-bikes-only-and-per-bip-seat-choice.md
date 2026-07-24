# 0004. Bipping out happens only on dirt bikes, and the player picks their seat each time

- **Status:** Accepted
- **Date:** 2026-07-24

## Context

Nothing in the design yet said how the player gets from **the Block** to a **rival set**
and back. The default would have been cars, which is what comparable games use.

Dirt bikes are authentic to the Chicago street culture the game draws from, and they turn
out to solve a design problem that had not yet been raised: a bike carries two, so the
size of **the ride** is capped by the fiction rather than by an imposed rule. The roster
decision stops being a squad-select menu and becomes the much more specific question of
who is on the back of your bike.

Bikes also suit the combat model from [ADR 0001](0001-tactical-agency-strategic-futility.md).
They are loud, fast, and offer no cover, which pushes the tension into arrival and
departure and keeps the **encounter** itself as brief as it needs to be.

Two questions followed. Whether bikes are the only option or one of several — a mixed
system was rejected because as soon as a car is available the bike becomes the risky
choice nobody takes, and one committed mechanic shapes level design far better than two
competing ones. And whether the player rides or shoots — where fixing either role
permanently was rejected in favour of a per-outing choice.

## Decision

We will make dirt bikes the only means of bipping out. There are no cars and no
alternative transport.

Before each **bip**, the player chooses their seat: ride the bike, or take the back and
shoot.

Riding means owning the approach and the escape while trusting a chosen set member's aim.
Taking the back means owning the violence while trusting a chosen set member to get you
out. Either way the player's own skill is genuinely decisive over half the outcome, and
the other half rests with someone they picked — which is
[ADR 0001](0001-tactical-agency-strategic-futility.md) expressed as a single control
decision, and it sharpens the roster stakes established in
[ADR 0002](0002-permanent-death-shrinking-set.md).

## Consequences

**Easier.** The ride's size needs no artificial cap. The roster choice gains a concrete,
legible frame — one seat, one person. Encounters stay short by construction, since there
is no cover to settle into.

**Harder.** Two distinct mechanics must both be excellent: riding and shooting. That is
real scope, and a weak version of either halves the game. Level design is constrained to
spaces bikes can use — streets, alleys, lots, gaps between buildings. Interiors and
stairs are effectively off the table unless the bike is parked, at which point it is a
spawn point rather than a mechanic.

**Principal risk.** When the player rides, an AI set member's shooting determines who
lives. If that AI reads as incompetent, players will experience deaths as the game
ignoring their input — which is precisely the failure mode ADR 0001 names as its trigger
for revisiting. The companion shooting AI is therefore load-bearing for the whole
design, not a secondary system.

**Worth revisiting if.** Playtesting shows one seat is strictly better than the other, or
players settle into a single seat and never switch. Either would mean the choice is
decorative, and the fix is to rebalance what each seat controls rather than to remove it.
