# Our Block — Design Spec

- **Status:** Draft
- **Date:** 2026-07-24
- **Sources:** [`CONTEXT.md`](../CONTEXT.md) for vocabulary; ADRs
  [0001](adr/0001-tactical-agency-strategic-futility.md)–[0008](adr/0008-flat-chapter-sequence.md)
  for the reasoning behind every decision below.

This document describes *what* the game is. It does not re-argue the decisions — where a
choice looks surprising, the linked ADR explains why it was made and what was given up.
Terms in **bold** are defined in `CONTEXT.md` and are used strictly.

---

## 1. Premise

**Jamal** is a native of **the Block**, a fictional South Side Chicago block. His **set** —
the eight people he grew up alongside — is in **beef** with a **rival set**. He is trying
to protect them.

He cannot. Every retaliation he wins produces the reason for the next one, and the set
gets smaller. He is not told this, and the game never states it. He experiences it.

The player's skill is real throughout. What the player is wrong about is not whether their
skill works, but what it accomplishes. They believe they are defending. They are
escalating. Because they keep winning, they never receive the feedback that would correct
them. ([ADR 0001](adr/0001-tactical-agency-strategic-futility.md))

## 2. Setting

The Block is invented — its name, its residents, and everything that happens on it. It is
recognizably drawn from the real **O-Block** in geography, architecture, and cultural
texture, and the drill-scene influence lives in music, dialogue, and camera language
rather than in real people's names.

No real person is a character in this game. No real event is depicted. This is a firm
constraint, not a preference.

## 3. The core loop

```
        OUTSIDE (on foot, time is scarce)
          spend limited time with some of the eight, not all
                    │
                    ▼
        beef flares — a reason to go
                    │
                    ▼
        PREPARE THE RIDE
          choose your seat: ride, or on the back
          choose who fills the other seat
                    │
                    ▼
        BIPPING OUT (on the bike)
          approach → ENCOUNTER (seconds, lethal) → escape
                    │
                    ▼
        RETURN — possibly one person fewer, permanently
                    │
                    └──────────► back to OUTSIDE
```

The loop's engine is that the last step supplies the motive for the next iteration. The
player retaliates because of who they lost retaliating.

**One turn of this loop is a chapter**, and the campaign is a fixed sequence of them. Every
chapter has the same shape — there are no acts and no mid-campaign turn. What changes
across the campaign is who is alive, who the player knows, and how far **the cycle** has
escalated, never the shape of the thing.
([ADR 0008](adr/0008-flat-chapter-sequence.md))

The flatness is load-bearing rather than a simplification. The cycle is a pattern that
feeds itself; a campaign that dramatises it by changing shape is describing it instead of
enacting it. It is also required by ADR 0001, which withholds the corrective feedback until
the end — a structural turn in the middle would be that feedback, delivered early.

Chapters are an authoring structure and are **never surfaced as a count**. No chapter
number, no progress bar, no position in a sequence.

## 4. Cast

**Jamal** — player character. Native to the Block; his history there predates the game.

**The set** — eight members. The player meets all eight within the first hour. There is no
recruitment and no replacement; the roster only shrinks.
([ADR 0002](adr/0002-permanent-death-shrinking-set.md))

Members are differentiated by **relationship and consequence, never by combat class or
loadout**. Each has a mild capability and a specific cost attached to their loss. The
intended shape of a roster decision is *"my best shooter is fifteen"* or *"the one who
never panics is the one whose mother feeds me"* — a moral choice wearing a tactical
costume.

The eight are **Alex, Tyrone, Anthony, James, Daquavion, Marcus, Omar, and Joshua**. Names
are settled; who these people are is not. See [`cast.md`](cast.md).

## 5. Mode: Outside

On foot on the Block. No bike, no weapons drawn, no destination. This is where the player
comes to know who anyone is, and — since there is no dialogue choice system — it is the
*only* place characterization happens. ([ADR 0005](adr/0005-scarce-time-outside.md))

**Structure.** A small, dense hub of a handful of locations, always the same, always
populated. Learned the way a resident learns a place. No quest markers, no objective list.
Set members are simply somewhere in it doing something; the player finds them by knowing
where they tend to be.

**Scarcity.** Time outside is limited. The player cannot come to know all eight. Who they
spend it with *is* the characterization mechanic — they will know three or four well and
the rest barely, and the ones they know are the ones they cannot afford to lose.

This is the mechanism by which the player authors their own tragedy rather than receiving
it. It is also why some deaths will land flat: that is the system working, and is not to
be "fixed" by guaranteeing every character an introduction.

**What knowing someone buys.** Nothing mechanical. A set member is exactly as capable on a
**bip** whether the player knows them well or has never spoken to them. What changes is
how readable that person is — a member the player knows can be anticipated, and one they
do not know is opaque. It is carried by the member's own behaviour and speech, never by a
meter, a marker, or any interface element.
([ADR 0006](adr/0006-familiarity-changes-legibility-not-competence.md))

Time outside therefore pays off on a bip without becoming experience points. The moment it
buys competence it becomes a resource to farm, the set becomes rankable on a single axis,
and the player starts spending time on who they intend to use rather than who they care
about — which is the one signal this whole mode exists to produce.

**Requirement.** The scarcity must read as a fact of life, not as a timer. If players
perceive a stress mechanic they will optimize against it instead of choosing honestly.

**How the budget ends.** One chapter is one budget of time outside plus the bip that closes
it, so the interval over which scarcity is spent and the chapter are the same thing. The
player chooses *when* to go, not *whether* — and nothing expires to make them. The things
to do outside simply run out, and going is what is left.
([ADR 0008](adr/0008-flat-chapter-sequence.md))

This is how the requirement above is met: the player is never pushed by a countdown, only
by having spent what they had.

## 6. Mode: Bipping out

### 6.1 Preparation

Two decisions, taken fresh before every **bip**:

1. **Your seat** — ride the bike, or take the back and shoot.
2. **Who fills the other seat** — a named member of the set.

Riding means owning the approach and the escape while trusting someone else's aim. Taking
the back means owning the violence while trusting someone else to get you out. Either way
the player's own skill is decisive over half the outcome, and the other half rests with a
person they chose. ([ADR 0004](adr/0004-dirt-bikes-only-and-per-bip-seat-choice.md))

These two decisions are the game's entire choice architecture (§8). The interface
presenting them is correspondingly load-bearing (§10).

### 6.2 The bike

Dirt bikes are the only means of bipping out. No cars, no alternatives. A bike carries
two, so **the ride** is capped by the fiction rather than by an imposed rule.

Bikes are loud, fast, and offer zero cover. The player is exposed the entire time they are
not moving, which places the tension in arrival and departure rather than in the fight.

Level design is constrained accordingly: streets, alleys, lots, gaps between buildings.
Interiors and stairs are effectively unavailable unless the bike is parked — at which
point it is a spawn point rather than a mechanic.

### 6.3 Encounters

An **encounter** is any exchange of gunfire. The model:

- Very low time-to-kill **in both directions**
- No regenerating health
- Duration measured in seconds, not minutes
- Two or three opponents on screen, not twenty

The real gameplay is the approach — who you brought, what you knew before you went, where
you came from, whether you are somewhere you can leave. By the time shots are fired the
outcome is largely determined by decisions made minutes earlier.

Low enemy counts are not a scope compromise. Anonymous waves dilute individual survival,
which is the specific failure mode ADR 0001 warns against. Skill must save *a named
person*, visibly, with a one-step causal line: you covered the stairwell, so Marcus made
it home.

### 6.4 Return

The player returns to Outside, possibly with one fewer member of the set. The loop repeats.

## 7. Death and consequence

Deaths of set members are **permanent**. No revival, no replacement, no late-game recovery.

**A death is never a fail state.** No "mission failed", no death screen, no reload prompt.
The game continues and the story absorbs the loss. This is not a UI preference — the
moment a death is framed as failure, players reload reflexively, permanence becomes
fiction, and an expensive system goes unexperienced. Nothing in UI, audio, or music may
present a set member's death as a losing outcome.

**Checkpointing is autosave-only.** Manual saves make save-scumming trivial and therefore
make permanence optional.

**Writing consequence.** Every scene after the first hour must work with an arbitrary
subset of eight people alive. This is the principal scope risk in the project. Mitigations
are the small roster, scenes authored to be robust to absence, and accepting reduced
specificity in later dialogue. Plot armor is explicitly rejected as a mitigation; if the
load proves unshippable, shrink the roster instead.

**When the set reaches zero.** The campaign has one ending, fixed by its chapter structure
rather than by the roster; how many are alive when it arrives is the variable. An empty set
reaches that same ending early — the loop cannot continue without someone to fill the other
seat, so the campaign proceeds to its ending from wherever it is. A player may finish with
any number from eight down to none.
([ADR 0007](adr/0007-zero-is-an-ending-not-a-fail-state.md))

Zero is not the destination and must not be inevitable — a campaign that simply runs until
everyone is dead is the pure tragedy ADR 0001 rejected. Nor is it a failure: it is the
first point at which the game's argument is legible, the player having performed well every
time and lost the set anyway.

**Nothing at the ending tallies the survivors** — no count, no score, no list of who made
it. A survival tally converts every death into a score in the final seconds, which is the
fail-state framing above arriving at the last possible moment. The ending is populated by
what is missing instead: each loss subtracts something from the Block permanently, so the
Block at the end is exactly as empty as the player made it.

## 8. Choice architecture

There is **no dialogue choice system**. No dialogue wheel, no branching conversation trees,
no relationship flags, no "X will remember that".
([ADR 0003](adr/0003-choice-lives-in-systems-not-dialogue.md))

All player choice is expressed through three systemic decisions, asked repeatedly under
escalating pressure against a roster that only shrinks:

| Decision | Where | Stakes |
|---|---|---|
| Who to spend time with | Outside | Determines who you will grieve |
| When to bip out | Outside → bip | Winning still feeds **the cycle** |
| Your seat, and who fills the other | Preparation | Whoever you bring may not return |

The second decision is *when*, not *whether* — the player cannot opt out of a bip, only
choose the moment ([ADR 0008](adr/0008-flat-chapter-sequence.md)). This is a narrowing of
how ADR 0003 originally framed it, and it is deliberate: an opt-out is a dominant strategy
in either direction, and **beef** is defined as a condition with no terms under which it
could be declared over. A condition is not something you decline.

A conversation-choice layer would compete with this for the player's attention — players
are trained to read dialogue prompts as "the choices" and a roster screen as a loadout,
which is exactly the inversion this design exists to prevent.

## 9. Explicitly foreclosed

Not omissions. Each is ruled out by a decision above, and re-adding any of them
contradicts it.

- **Crime-empire progression** — territory capture, growing the set, consolidating power,
  any structure where the beef can be won outright (ADR 0001)
- **Recruitment or replacement** of set members; story-critical plot armor (ADR 0002)
- **Dialogue choices** and relationship-flag bookkeeping (ADR 0003)
- **Cars and alternative transport** (ADR 0004)
- **Guaranteed introductions** to every set member (ADR 0005)
- **Familiarity as a stat** — meters, tiers, or any interface element surfacing a
  companion's intent; any competence that varies with how well the player knows someone
  (ADR 0006)
- **Last-member protection, bipping out alone, multiple endings ranked by roster count,
  and survivor tallies** (ADR 0007)
- **Acts and mid-campaign structural turns**; any beat delivering the design's argument
  before the ending; variable-length chapters containing multiple bips; surfaced chapter
  counts or progress indicators; forcing a bip by countdown (ADR 0008)
- **Manual saves** (§7)
- **Real people and real events** from O-Block (§2)

## 10. Load-bearing risks

Both are flagged inside ADRs where they are at risk of being treated as polish. They are
not.

**Companion shooting AI.** When the player rides, an AI set member's aim decides who
lives. If that AI reads as incompetent, players will experience deaths as the game
ignoring their input — precisely the trigger ADR 0001 names for revisiting the whole
thesis. This is a primary system.

ADR 0006 narrows this risk without removing it: because familiarity never affects
competence, no companion is ever deliberately authored as bad, so the problem is holding
one high bar rather than shipping members who fall below it on purpose. What ADR 0006 adds
in its place is behavioural variation between members the player knows and members they do
not — real animation, audio, and VO scope, and the kind that gets cut first because
nothing visibly breaks when it is missing.

**The roster interface.** ADR 0003 placed the game's entire choice architecture on one
screen. It must be exceptionally readable and must surface relationships and history
continuously. If it feels like an inventory or loadout screen, the design collapses. This
is the highest-risk UI in the project.

Secondary: low time-to-kill makes checkpointing and readability make-or-break; short
encounters mean the preparation layer must carry the minute-to-minute weight, and a thin
prep layer means a thin game.

## 11. Open questions

None of these threaten what is settled above. All need answers before production.

1. **Engine.** No engine chosen; no code exists. Deliberately deferred — the decisions
   above are engine-agnostic. Warrants its own ADR when taken.
2. **The Block's fictional name.** Unnamed.
3. **What started the beef.** Undefined. Note that **beef** is defined as having no start
   date every participant would name the same way — so this may be answered as
   deliberately contested rather than as a fact.
4. **The eight themselves.** Each needs a capability and, more importantly, a specific
   cost-of-loss. This is the next substantial design work. Names are settled — see
   [`cast.md`](cast.md).
5. **How scarcity is expressed.** Narrowed by ADR 0008, not closed. The *unit* is settled —
   one chapter is one budget of time outside — and so is the fact that the budget running
   out, rather than a clock, is what sends the player. What that budget is made of in the
   fiction, and how a player perceives it running low without perceiving a timer, is still
   open. This is the remaining half of ADR 0005's hardest requirement.
6. **How the beef flares.** What generates a reason to bip out. The other half of this
   question — whether the player can decline indefinitely — is settled by ADR 0008: they
   cannot.
7. **The escalation curve.** ADR 0008 gave it a domain to be drawn across, and ADR 0001
   fixes that it is monotonic and that winning still escalates. Its shape remains
   deliberately undecided.
8. **Loss rate per bip.** Tuning rather than decision, but it must be tuned jointly with
   chapter count — ADR 0008 makes the two a single knob, since together they determine
   whether an empty set stays reachable and unlikely.
