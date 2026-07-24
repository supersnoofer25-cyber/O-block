# 0002. Set members die permanently, the set never grows, and a death is never a fail state

- **Status:** Accepted
- **Date:** 2026-07-24

## Context

[ADR 0001](0001-tactical-agency-strategic-futility.md) commits the game to a rising
campaign-level cost that the player cannot prevent through skill. That cost has to be
paid in something the player can see. Combined with lethal **encounters** and a **ride**
whose members are differentiated by relationship rather than by combat class, the obvious
currency is the members of the **set** themselves.

This forces a decision that is not a tuning value, because the entire writing
architecture depends on it: when someone on the ride is killed, are they gone?

The alternatives were genuinely on the table.

**"Downed, not dead."** Injuries and absences, consequences short of permanence. Far
cheaper to write, keeps the cast intact for every scene, and avoids save-scumming
entirely. Rejected because the roster then never shortens, the cycle has no visible
price, and dialogue would have to assert a cost the systems never demonstrate.

**Permanent death with story-critical exemptions.** A middle path where certain
characters cannot die. Rejected as a compromise that undercuts the mechanic it is
protecting — players detect plot armor quickly, and once detected it retroactively
cheapens every death that was real.

A related question is what happens after a loss. If a set member's death is presented as
failure, players reload reflexively, permanence becomes fiction, and an expensive system
goes unexperienced by most of the audience. Permanence is therefore not viable as a
standalone decision; it only works alongside a commitment about framing.

## Decision

We will make set member deaths permanent, and we will support that with two structural
commitments.

**The set never grows.** The player meets everyone they will ever have within the first
hour — roughly eight people — and there is no recruitment and no replacement. The number
only goes down. The cycle is thereby expressed as a system rather than asserted in
dialogue: the player watches the roster shorten and infers the cost of the beef without
being told it.

**A death is never a fail state.** No "mission failed," no death screen, no reload
prompt when a set member is killed. The game continues and the story absorbs the loss.
Losing someone is the story proceeding, not the player underperforming.

## Consequences

**Easier.** The central theme needs no exposition; the roster proves it. Roster choice
before each **bip** carries genuine weight, because the player is spending people who do
not come back. Every playthrough diverges structurally rather than cosmetically.

**Harder.** The writing burden is combinatorial: every scene after the first hour must
work with an arbitrary subset of eight people alive. This is the principal reason most
games avoid permanent death, and it is the main risk to the project's scope. Mitigations
are a deliberately small roster, scenes written to be robust to absence, and accepting
reduced specificity in later dialogue. Checkpointing must be autosave-only, since manual
saves make save-scumming trivial and permanence optional.

**Foreclosed.** Recruitment, replacement characters, and any late-game roster recovery.
Also foreclosed: presenting a set member's death as a losing outcome anywhere in the UI,
audio, or music.

**Accepted risks.** Some players will save-scum regardless, particularly early; this can
be discouraged but not prevented, and the counter is to make the aftermath compelling
enough that continuing beats reloading. Some portion of the audience and press will read
the design as punishing. Both were judged acceptable.

**Worth revisiting if.** The combinatorial writing load proves unshippable at eight
members. The correct response is to shrink the roster, not to introduce plot armor.
