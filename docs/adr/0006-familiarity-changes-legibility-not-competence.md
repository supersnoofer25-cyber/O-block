# 0006. Familiarity changes how legible a companion is, not how competent they are

- **Status:** Accepted
- **Date:** 2026-07-24

## Context

[ADR 0005](0005-scarce-time-outside.md) made time **outside** scarce so that who **Jamal**
spends it with becomes the characterization mechanic. That produces a per-member quantity
— how well the player knows someone — which the campaign-state spec provisionally calls
*familiarity*.

[ADR 0004](0004-dirt-bikes-only-and-per-bip-seat-choice.md) put a chosen set member in the
other seat on every **bip**, where their performance decides who lives. So the two systems
meet, and nothing yet said whether they touch: does knowing someone better make them
better on a bip?

The question cannot be deferred. Both quantities live in the same module, and the cast
contract asserts that a member's **capability** and their **cost of loss** are independent
properties. If familiarity feeds competence, that independence is gone.

Three options were genuinely on the table.

**Familiarity improves competence.** The people you know shoot straighter and ride better.
Attractive because the shape the design wants falls out for free — the person you most
want on the back becomes the person you can least afford to lose, with nothing authored
per member.

It was rejected for three compounding reasons. It makes time outside instrumental: the
player stops spending it on who they care about and starts spending it on who they intend
to bring, which destroys the exact signal ADR 0005 exists to produce. It re-ranks the set
on a single axis, since familiarity is a common scale every member sits on, restoring the
loadout comparison [ADR 0003](0003-choice-lives-in-systems-not-dialogue.md) forecloses.
And it compounds: the player invests in three or four, brings those three or four, loses
those three or four, and finishes the campaign with strangers who are also mechanically
worse — an emotionally flat back half that is simultaneously the hardest to play. That
last one is ADR 0001's named trigger arriving as a difficulty spike at the exact point the
story is heaviest.

It also forces the project to ship deliberately incompetent companion AI for members the
player does not know, which is precisely the failure ADR 0004 names as load-bearing for
the whole design.

**Familiarity is purely emotional.** Competence is fixed per member; time outside has no
expression on a bip at all. Safe, and it protects everything above. Rejected as leaving
real value unclaimed: half the game happens on a bip, and a player who sees no trace there
of the time they spent may reasonably conclude that outside is the part of the game that
does not count.

**Familiarity changes legibility.** Competence is untouched; what changes is the player's
ability to read the person next to them.

## Decision

We will not let familiarity affect a companion's competence. A member is exactly as good
on a bip whether the player knows them well or has never spoken to them.

What familiarity changes is how legible that companion is. A member the player knows is
someone they can anticipate — what he is about to do, what he does when it goes wrong,
when he is in trouble. A member the player does not know is opaque, and the player rides
with someone whose next move they cannot predict.

This is expressed entirely through the companion's own behaviour, communication, and how
much he tells the player unprompted. It is never expressed through UI. There is no
familiarity meter, no intent marker, no indicator that surfaces what a companion is about
to do — any of those would be a competence buff wearing a costume, and would restore the
common axis this decision exists to remove.

The effect therefore routes through the player's skill rather than around it. Familiarity
supplies information; converting information into a living companion is the player's job,
which is [ADR 0001](0001-tactical-agency-strategic-futility.md)'s individual-scale agency
applied to the one system that was at risk of bypassing it.

## Consequences

**Easier.** Capability and cost of loss stay genuinely independent, so the cast contract
holds and the roster cannot be ranked. Time outside pays off on a bip without becoming
experience points, so the scarcity keeps reading as a fact of life rather than a resource,
which is what ADR 0005 requires of it. Companion AI never has to be authored as bad —
every member is competent, and ADR 0004's principal risk is defused rather than managed.
The discomfort of riding with someone you never got to know is produced by the fiction
instead of by a penalty, so it survives ADR 0002's prohibition on framing loss as failure.

**Harder.** This is a subtle effect that has to land without a HUD to announce it.
Behavioural variation per member, in animation, audio, and how they speak on a bip, is
real scope, and it is the kind of scope that gets cut first because nothing visibly breaks
when it is missing. It also has to be strong enough that players notice it at all, and we
have deliberately removed the cheapest way of making them notice.

**Foreclosed.** Familiarity as a stat, a meter, a tier, or anything a player can read as
progress. Any UI affordance that surfaces a companion's intent. Any per-member competence
that varies with player investment.

**Worth revisiting if.** Playtesting shows players cannot tell the difference between
riding with a stranger and riding with someone they know. The response is to strengthen
how the companion expresses himself — not to add a competence delta, which would trade a
presentation problem for the structural collapse described above.
