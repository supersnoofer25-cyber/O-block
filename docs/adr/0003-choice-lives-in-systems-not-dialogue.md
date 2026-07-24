# 0003. Choice lives entirely in the systems layer; there is no dialogue choice system

- **Status:** Accepted
- **Date:** 2026-07-24

## Context

*Our Block* is pitched as narrative-driven and built on high-stakes choices. The
conventional way to deliver that in a narrative action game is a conversation-level
choice layer — dialogue wheels, branching responses, relationship flags. It is expected
by the audience, it is legible to press and marketing, and it gives writers a direct
instrument for character.

By the time this decision came up, the design already produced two recurring choices as a
consequence of earlier commitments:

- **Do you bip out?** — under [ADR 0001](0001-tactical-agency-strategic-futility.md),
  winning an exchange still feeds **the cycle**, so choosing to retaliate is choosing to
  make things worse.
- **Who is on the ride?** — under
  [ADR 0002](0002-permanent-death-shrinking-set.md), whoever the player brings may not
  come back, and cannot be replaced.

The real question was therefore not whether the game has meaningful choices, but whether
to add a second, more conventional choice system alongside the one the systems already
generate.

## Decision

We will not build a dialogue choice system. No dialogue wheel, no branching conversation
trees, no "X will remember that" relationship flags.

All player choice in the game is expressed through the two systemic decisions above,
asked repeatedly under escalating pressure against a roster that only shrinks.

The reasoning is not minimalism for its own sake. A conversation-choice layer would
actively compete with the systemic one for the player's attention. Players are trained to
read dialogue prompts as "the choices" and a roster screen as a loadout — precisely the
inversion this design exists to avoid. Keeping choice entirely in the systems layer
means the player cannot mistake where the weight sits.

## Consequences

**Easier.** The narrative pipeline is simpler: scenes are authored linearly against a
known set of surviving characters rather than against a branch tree crossed with a
survival state. No relationship-flag bookkeeping. The game's thesis stays legible,
because there is only one place consequence comes from.

**Harder.** The roster screen now carries the game's entire choice architecture. It must
be exceptionally readable, must surface relationships and history continuously, and must
never feel like an inventory or loadout screen. If that interface is merely functional,
the design collapses — this is the highest-risk piece of UI in the project.

**Given up.** The conventional "your choices matter" marketing hook, which is a genuine
commercial consideration for a narrative title. Writers also lose their most direct
instrument for expressing character, and must carry characterization through
non-interactive scenes, ambient dialogue, and behavior on a bip instead.

**Worth revisiting if.** Playtesting shows players do not perceive the roster decision as
a choice at all. The correct response is to invest further in the roster interface and in
surfacing relationships — not to add a dialogue layer, which would compete with the
mechanic rather than reinforce it.
