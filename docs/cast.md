# Cast

Player character: **Jamal** — see [`CONTEXT.md`](../CONTEXT.md).

The **set**, eight members:

**Alex · Tyrone · Anthony · James · Daquavion · Marcus · Omar · Joshua**

Names are settled. What each of these people *is* — a capability and a specific
cost-of-loss — is not yet authored; that is the next substantial design work, and the
contract it must satisfy is [issue #4](https://github.com/supersnoofer25-cyber/O-block/issues/4).

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
