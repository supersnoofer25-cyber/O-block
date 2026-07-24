# Our Block — Design Spec

- **Status:** Draft
- **Date:** 2026-07-24
- **Sources:** [`CONTEXT.md`](../CONTEXT.md) for vocabulary; ADRs
  [0001](adr/0001-tactical-agency-strategic-futility.md)–[0005](adr/0005-scarce-time-outside.md)
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

Current names are **placeholders** pending rename; see
[`cast-placeholders.md`](cast-placeholders.md).

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

**Requirement.** The scarcity must read as a fact of life, not as a timer. If players
perceive a stress mechanic they will optimize against it instead of choosing honestly.

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

## 8. Choice architecture

There is **no dialogue choice system**. No dialogue wheel, no branching conversation trees,
no relationship flags, no "X will remember that".
([ADR 0003](adr/0003-choice-lives-in-systems-not-dialogue.md))

All player choice is expressed through three systemic decisions, asked repeatedly under
escalating pressure against a roster that only shrinks:

| Decision | Where | Stakes |
|---|---|---|
| Who to spend time with | Outside | Determines who you will grieve |
| Whether to bip out | Outside → bip | Winning still feeds **the cycle** |
| Your seat, and who fills the other | Preparation | Whoever you bring may not return |

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
- **Manual saves** (§7)
- **Real people and real events** from O-Block (§2)

## 10. Load-bearing risks

Both are flagged inside ADRs where they are at risk of being treated as polish. They are
not.

**Companion shooting AI.** When the player rides, an AI set member's aim decides who
lives. If that AI reads as incompetent, players will experience deaths as the game
ignoring their input — precisely the trigger ADR 0001 names for revisiting the whole
thesis. This is a primary system.

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
4. **The rename.** Eight placeholder names to replace; Connor/Collin collide and must be
   split. See [`cast-placeholders.md`](cast-placeholders.md).
5. **The eight themselves.** Each needs a capability and, more importantly, a specific
   cost-of-loss. This is the next substantial design work.
6. **How scarcity is expressed.** That time outside is limited is settled; whether it is
   days, visits, or fictional pressure is not.
7. **Campaign length and chapter structure.** Undefined.
8. **How the beef flares.** What generates a reason to bip out, and whether the player can
   decline indefinitely.
