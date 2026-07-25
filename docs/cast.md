# Cast

Player character: **Jamal** — see [`CONTEXT.md`](../CONTEXT.md).

The **set**, eight members:

**Alex · Tyrone · Anthony · James · Daquavion · Marcus · Omar · Joshua**

Names are settled. Capabilities are mostly settled; costs of loss mostly are not. The
contract all of it must satisfy is
[issue #4](https://github.com/supersnoofer25-cyber/O-block/issues/4).

## The eight

**A capability is a behaviour, never a rating, and it is defined in both seats.** This is
the rule the whole section runs on. "Good at shooting, bad at driving" is a score on the
game's two verbs, and eight people scored on two axes is the loadout screen
[ADR 0003](adr/0003-choice-lives-in-systems-not-dialogue.md) exists to prevent. Worse, it
collapses two decisions into one: pick your seat and the companion is chosen for you,
which is the failure [ADR 0004](adr/0004-dirt-bikes-only-and-per-bip-seat-choice.md) names.
So each man is one trait that shows up whichever seat he takes.

| | Capability | Cost of loss |
|---|---|---|
| **Alex** | **Doesn't hesitate.** On the back he fires first, before you've decided this is happening. Riding, he commits to the approach — closer than you'd have chosen. | **He has a little sister.** After Alex she stops coming outside. She is still on the Block; you just don't see her any more. |
| **Tyrone** | **Won't shut up.** Riding, he's calling what's ahead; on the back, what's behind. You always know what he's seeing. | **He sells food.** After Tyrone the Block doesn't eat the way it did. |
| **Anthony** | **The one who says no.** Riding, he takes the careful exit. On the back, he holds until he's sure. | **He has a job.** After Anthony nobody on the Block is working — the thread to an ordinary life goes with him. |
| **James** | **Doesn't leave early.** On the back he keeps firing until it's actually done. Riding, he won't pull out while anyone is still exposed. | **He fixes the church roof.** After James it doesn't get fixed. The church stands and people use less of it. |
| **Daquavion** | **Moves first.** Reacting, not dodging — when it goes wrong he is already moving. | **He has a newborn.** After Daquavion, his girl takes the baby off the Block. Two people who were part of the place are gone from it. |
| **Marcus** | **Knows the way out.** Riding, he takes exits you wouldn't have found. On the back, he's telling you where to go before you've asked. | **He works on cars.** After Marcus they stop running — not his, everyone's. The Block loses the man who kept it moving. |
| **Omar** | **Acts like nothing is happening.** Riding, he never tenses up. On the back, he's the calmest gun in the set, because he hasn't quite registered that it's real. | **He has a basketball hoop.** After Omar nobody plays. It stands there and nobody uses it. |
| **Joshua** | **Doesn't flinch.** Too young to have learned to. Riding, he goes where you point without asking; on the back, he doesn't look away. | **He lives with Jamal.** After Joshua, the house Jamal comes back to is empty. |

### Notes

**Alex and Anthony are deliberate opposites** — commit early against hold until sure —
without being a comparison of ratings. Either can be wrong on a given **bip**.

**Tyrone is legible to everyone**, including a player who never spent time with him. That
is a real interaction with
[ADR 0006](adr/0006-familiarity-changes-legibility-not-competence.md) rather than a
conflict with it: he is the member a player who knows nobody can still bring.

**James is set against Marcus.** Marcus knows the way out; James won't take it yet. Both
are right sometimes.

**Daquavion is the shape `spec.md` §4 was reaching for.** He is the best man to bring —
already moving when it goes wrong — and he has the most waiting for him. Nothing about
*moves first* tells you about the newborn, so a player optimising the roster cannot see the
price coming. Bringing him is the decision the whole game is about.

**Joshua's cost is the only one paid in Jamal's own space.** Every other cost is observed
elsewhere on the Block; his cannot be routed around by not going somewhere, and it is paid
at the start of every chapter for the rest of the campaign. It also partly answers
[#4](https://github.com/supersnoofer25-cyber/O-block/issues/4)'s open question 4 — a cost
of loss does not remove a location, it hollows one. That is likely the general answer.

**Alex and Daquavion are deliberately differentiated.** Both are a man dying and a
dependent going, which risks the player being unable to tell whose absence caused what
(story 26). They differ in *what* goes, not who: Daquavion's girl and baby leave the Block
entirely; Alex's sister stays and stops coming **outside**.

**Marcus's cars must not read as available transport.** ADR 0004 forecloses cars as a means
of **bipping out**, and putting a mechanic on the Block invites *"why can't I take one?"*
These are residents' cars — someone's mother's — not the set's. That has to be authored
deliberately and visibly rather than left for a player to test.

**Costs of loss come from what a man is on the Block, not from what he does on a bip.**
A capability is exercised **bipping out**; a cost of loss is paid **outside**. Deriving one
from the other collapses them into a single fact about a person and hands a player
optimising the roster a way to rank it. The shape to aim for is `spec.md` §4's own — *the
one who never panics is the one whose mother feeds me* — where the two facets are
unrelated. Tyrone landed on that line almost exactly, except that it is him.

## Time outside

Spending time with a member is not talking to him. Each is doing a particular thing in a
particular place, and time with him means doing it alongside him —
[ADR 0005](adr/0005-scarce-time-outside.md)'s *somewhere in it doing something* and *where
they tend to be*, made concrete.

| | Time with him is | Which is why his death is felt |
|---|---|---|
| **Alex** | Around, with his sister there | You knew her, so you notice she stops coming out |
| **Tyrone** | At his spot while he sells | You ate there, so you notice nobody does |
| **Anthony** | Caught before or after work | You saw him leaving, so you notice nobody leaves |
| **James** | Up on the church roof with him | You were up there, so you notice it stops getting fixed |
| **Daquavion** | At his place, around the baby | You held her, so you notice they are gone |
| **Marcus** | Under a car | You worked on them, so you notice they stop running |
| **Omar** | At the hoop | You played, so you notice nobody plays |
| **Joshua** | At home, because you live there | You lived with him, so you notice the house |

**The activity is not free choice — it is the man's cost-of-loss material.** That is what
makes the third column work. A player who never went to the hoop learns that Omar is dead
and sees nothing change; a player who played there every chapter watches it stand empty.
This is the mechanism behind familiarity gating recognition of a cost, which
[ADR 0009](adr/0009-behaviour-and-absence-never-a-number.md) requires and which nothing
else explains how to deliver.

**It is also the answer to per-chapter monotony.** There are eight distinct things to do
outside and the player does a different handful each chapter, because who they choose
changes. Variety comes from their own choice rather than from a task list, which
[ADR 0005](adr/0005-scarce-time-outside.md) forecloses and which would pull attention from
people onto objectives — tasks can be completed and people cannot.

## Ties

A tie is something between two members, so that when one dies the other changes. Without
them the eight are eight strangers who live near each other; with them a death ripples
instead of removing a name.

**Four ties, each man in exactly one.** Eight men fully connected is twenty-eight pairs,
each needing an authored consequence in both directions and in any order of deaths — the
combinatorial load [ADR 0002](adr/0002-permanent-death-shrinking-set.md) names as the
project's principal scope risk. Four pairs is eight ripples to write, which is
shippable.

**Every tie is harvested, not invented.** Each of these was already true given what the two
men are; none was added to reach a number. Ties invented to fill a quota read as
constructed, and cost writing that the emergent ones do not.

| Tie | Why it was already there |
|---|---|
| **Anthony — Marcus** | Anthony has a job off the Block. Marcus keeps the cars running. Anthony needs a car to get to work. |
| **Tyrone — James** | Tyrone sells food. James keeps up the church, which is where the Block gathers. He feeds people where they come together. |
| **Omar — Joshua** | Omar has the hoop. Joshua is the youngest. The kid is where the game is. |
| **Alex — Daquavion** | The two with someone waiting at home — a little sister, a newborn. They are the ones who leave early. |

**Alex and Daquavion were the near-collision, and the tie is what fixes it.** Both costs
read as *a man dies, a dependent goes*, which risked story 26's requirement that each loss
be attributable. Pairing them deliberately turns the resemblance into the structure: they
are alike because they are the two with someone at home, and that is the thing between them.

**Ripples reach direct ties only and stop there** (story 32). Marcus dying changes Anthony;
it does not change Tyrone. A single death cannot cascade into rewriting the set.

**Jamal's ties are a separate axis.** Every member has a relationship to him — that is
story 1, and it is not what this graph counts. Joshua is in both: tied to Omar here, and
Jamal's brother besides.

**A cost hollows a place; it never removes one.** James's church stands and is used less.
Joshua's house is still there and is empty. Omar's hoop is still up. If costs could delete
locations the hub would shrink as the campaign runs, and
[ADR 0005](adr/0005-scarce-time-outside.md) needs it small, dense, and legible enough that
finding people is knowledge rather than searching. This answers
[#4](https://github.com/supersnoofer25-cyber/O-block/issues/4)'s open question 4.

**No two costs subtract the same kind of thing.** A service, a rhythm, mobility, a
gathering place, play, a departure, a withdrawal, a home. Story 26 requires each loss be
attributable to a specific man's absence, which fails the moment two of them read alike —
the near-misses were Alex against Daquavion, and Marcus against James.

**Taken together they compose, and that is the point.** Run a costly campaign and the Block
loses its food, its work, its mobility, its gathering place, its play, its children, and
finally the house Jamal comes home to. What is left is the **beef** and nothing else. This
is what [ADR 0007](adr/0007-zero-is-an-ending-not-a-fail-state.md) meant when it said the
ending is populated by what is missing and the Block is exactly as empty as the player made
it — the ending needs no additional system, because these eight subtractions *are* it.

**Joshua is Jamal's younger brother**, and he **starts known** — full familiarity from the
first hour, with no time **outside** required. You cannot not know your own brother.

This looks like the special case the contract forbids and is not one: the contract governs
which *properties* a member carries, and every member carries the same ones. Joshua's is a
starting value. [ADR 0005](adr/0005-scarce-time-outside.md)'s concern was that knowing all
eight equally flattens the roster into a capability comparison, and one pre-known member
flattens nothing. It also gives the design exactly one death guaranteed to land, which
hedges ADR 0005's accepted risk that some land flat.

**Joshua is not exempt from death**, and can be lost on the first bip.
[ADR 0002](adr/0002-permanent-death-shrinking-set.md) forecloses plot armour by name and is
explicit that once players detect it anywhere, every other death is retroactively
cheapened. He is the character who will test that commitment hardest, and that is chosen
rather than inherited. Every scene must also work with the subset where he died nine
chapters ago.

**Joshua answers [#4](https://github.com/supersnoofer25-cyber/O-block/issues/4)'s open
question 5** — Jamal is in the tie graph, and this is its strongest edge.

## Why these names

The **set** is defined as the people who grew up on **the Block** together, with
membership a fact of where you're from rather than something you join. There is no
selection mechanism in that definition, so the set's composition is simply the Block's
composition — and the Block is recognizably drawn from Parkway Gardens.

An earlier placeholder set (Connor · Cy · Grayson · Declan · Jack · Trevor · Michael ·
Collin) did not read that way, which left the fiction owing an account of how a
block-level set ended up looking like that. Those were placeholder names rather than
intent, so the rename closes it and no such account is owed.

Deliberately **not** recorded as an ADR: this is the unsurprising direction the existing
definition of **set** already pointed, and nothing was given up. There is no trade-off
worth preserving the reasoning for.

## Distinctness

Under [ADR 0002](adr/0002-permanent-death-shrinking-set.md) the player must tell eight
people apart instantly while deciding who rides on the back and who might not come home.
Two rules follow, and any future change to this list must keep both:

- **Distinct on first syllable** — AL, TY, AN, JAMES, DA, MAR, O, JOSH.
- **Distinct in written silhouette** — no two names confusable at a glance.

The list has two shared initials, *Alex/Anthony* and *James/Joshua*. Both pairs differ
clearly on first syllable and in length, so neither is a collision. The bar is the
Connor/Collin failure — same first syllable, near-identical shape — and nothing here
meets it.

Two candidates were dropped for exactly that reason. *Tyreese* collided with *Tyrone* on
both rules; *Demarcus* collided with *Daquavion*, both landing as "duh-" in speech at
similar length.

**Minor, for the ear rather than the eye:** *Omar* and *Marcus* share a stressed "mar".
They pass both written rules and differ on first syllable, so this is a note for VO
casting, not a defect.
