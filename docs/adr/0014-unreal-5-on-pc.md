# 0014. Unreal 5, PC first, chosen on character behaviour rather than on rendering

- **Status:** Accepted
- **Date:** 2026-07-25

## Context

`spec.md` §11 deferred the engine deliberately and was right to: every decision taken so
far is engine-agnostic, and taking this one early would have bought nothing. That deferral
has now run out. The text prototype has answered everything it can answer — it settled the
chapter count and turned up two decisions that were written down and never built — and the
question it cannot reach is the one that matters most. `cmd/roundhill` *rolls* for whether
a companion comes home. [ADR 0001](0001-tactical-agency-strategic-futility.md) promises the
player that playing well saves the person in front of them, and nothing can test that
promise until there is an encounter, which means until there is an engine.

**What actually decides this, and it is not rendering.** Three decisions independently put
the design's weight in the same place:

- [ADR 0006](0006-familiarity-changes-legibility-not-competence.md) makes familiarity show
  as *legibility* — what a companion tells you unprompted, whether you can anticipate him —
  and forbids expressing any of it through UI.
- [ADR 0009](0009-behaviour-and-absence-never-a-number.md) makes a capability a behaviour
  rather than a rating, which has to be legible in play with no number and no interface
  element to announce it.
- [ADR 0004](0004-dirt-bikes-only-and-per-bip-seat-choice.md) names companion AI as
  load-bearing for the whole design.

All three land on animation, audio, VO and companion AI. Both ADR 0006 and ADR 0009 go
further and name this as *the kind of scope that gets cut first because nothing visibly
breaks when it is missing*. An engine choice made on lighting or on world size would be
optimising the part of this game that is least at risk.

**The constraint that decides it in practice.** One developer, with AI assistance, and
existing Unreal experience. For a solo build the scarcest resource is not compute or
licensing, it is the developer's hours, and learning an unfamiliar engine is paid out of
exactly that account — while delivering nothing the design asked for.

**The alternatives were real.** Unity iterates faster, runs lighter, and has the larger
asset ecosystem; its character animation fidelity takes more assembly to reach. Godot has
no licence terms and the fastest edit-run loop of the three, and the weakest 3D character
animation tooling — which is precisely where this design puts its weight. Both were
rejected on the same two grounds: neither is better at the thing that decides this game,
and both would charge a solo developer an engine-learning tax first.

Continuing to defer was also on the table and was rejected. The deferral was justified
while the open questions were answerable without an engine. They no longer are.

## Decision

We will build in Unreal 5, targeting PC first.

**Chosen for its character tooling.** MetaHuman, Motion Matching, Control Rig and animation
blueprints target the companion-behaviour scope the design leans on hardest. That is the
whole argument. Nanite and Lumen are conveniences here, not reasons.

**Existing experience is a legitimate deciding factor and is recorded as one.** It is not a
tiebreaker applied after the technical case; for a solo developer it is most of the case.

**PC first**, because it removes certification from the critical path and keeps the
iteration loop short. Console does not reopen this decision if it comes later.

**The `campaign` module is ported, not bound.** It is Go and Unreal is C++. The module was
built headless, pure, and dependency-free precisely so it could outlive this decision, and
it is roughly six hundred lines whose entire interface is one function. Porting it is
cheap; running two toolchains and a language boundary for the rest of the project's life is
not. The Go test suite ports with it and remains the specification — it encodes the stories
rather than the implementation, so it is the part worth keeping most.

## Consequences

**Easier.** The behavioural work the design rests on has first-class tooling to land in
rather than a pipeline to invent, and it is the one part of this project where being under-
resourced is fatal rather than merely slow. Blueprints let one person iterate on companion
behaviour without a second programmer. Environment fidelity stops being a bottleneck for a
solo developer, which matters for a game whose setting is a single dense Block that
[ADR 0005](0005-scarce-time-outside.md) needs legible enough that finding people is
knowledge rather than searching.

**Harder.** Unreal is heavy for one person: long builds, an editor that expects a team, and
a C++ layer waiting at the point where Blueprints run out. The royalty applies past the
usual gross threshold, which is a real term even if it is a good problem to have.

**The engine does not touch this project's actual risk, and choosing one may disguise it.**
Eight members, each needing a distinct behaviour legible in both seats and at three
readings, plus per-chapter ambient content across twelve chapters, plus an encounter, is a
studio-sized content load arriving at a solo developer. No engine reduces it. ADR 0008
already named the levers and their order — bounding ties, then chapters, then the roster —
and this decision changes none of that. If anything it sharpens it: Unreal makes it
possible to build the eight *well*, which makes building too many of them the likelier
failure.

**The marketplace is a specific trap here.** Bought animation sets are fine for the Block
and dangerous for the eight. ADR 0009 is explicit that a member who is not working cannot be
fixed by adjusting a value, only by rewriting who he is — and a generic locomotion set
applied across the roster would flatten exactly the differentiation ADR 0003 put the game's
whole choice architecture on. Environment assets, yes. The eight, no.

**Foreclosed.** Unity, Godot, and a custom engine. Re-deciding the engine once production
content exists — the point of taking this now is that everything before it was written to
be engine-agnostic and everything after it will not be.

**Worth revisiting if.** The companion-behaviour work proves undeliverable at this team
size. The response is the one ADR 0008 and
[ADR 0002](0002-permanent-death-shrinking-set.md) already specify — cut ties, then
chapters, then the roster — and not to change engine, which would cost everything already
built and reduce the content load by nothing.
