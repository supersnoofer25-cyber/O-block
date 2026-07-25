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
| **Tyrone** | **Won't shut up.** Riding, he's calling what's ahead; on the back, what's behind. You always know what he's seeing. | *unauthored* |
| **Anthony** | **The one who says no.** Riding, he takes the careful exit. On the back, he holds until he's sure. | *unauthored* |
| **James** | **Doesn't leave early.** On the back he keeps firing until it's actually done. Riding, he won't pull out while anyone is still exposed. | **After James, going out is harder for everyone.** Permanently. |
| **Daquavion** | **Moves first.** Reacting, not dodging — when it goes wrong he is already moving. | **He has a newborn.** After Daquavion, his girl takes the baby off the Block. Two people who were part of the place are gone from it. |
| **Marcus** | **Knows the way out.** Riding, he takes exits you wouldn't have found. On the back, he's telling you where to go before you've asked. | **He works on cars.** After Marcus they stop running — not his, everyone's. The Block loses the man who kept it moving. |
| **Omar** | **Acts like nothing is happening.** Riding, he never tenses up. On the back, he's the calmest gun in the set, because he hasn't quite registered that it's real. | **The highest in the set.** He is the reason it is bearable — being outside with him is the only time the game isn't about the **beef**. He is what you were protecting. |
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
unrelated.

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
