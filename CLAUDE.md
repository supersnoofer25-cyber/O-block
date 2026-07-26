# Working on Our Block

A narrative-driven third-person action-adventure: tactical urban shootouts, high-stakes
choices, a fixed cast of eight who can die permanently.

This project is **design-led**. Almost everything is decided and written down before code
exists, and the decisions are binding. Read before building.

## Where things live

| Path | What it is |
|---|---|
| `docs/spec.md` | The design spec. §11 holds the remaining open questions. |
| `docs/adr/` | Fifteen decision records. These are binding, including their foreclosures. |
| `docs/cast.md` | The eight, in prose. `campaign/cast.go` is that file as data. |
| `CONTEXT.md` | The glossary. Terms have exact meanings — use them strictly. |
| `campaign/` | Headless Go rules module. Pure, engine-agnostic, no rendering, no I/O. |
| `cmd/roundhill/` | Text prototype. Not the game — a harness for answering design questions cheaply. |

## Commands

```bash
go test ./...                        # the story-numbered test suite
go vet ./... && gofmt -l .
go run ./cmd/roundhill               # play it (interactive; needs a real terminal)
go run ./cmd/roundhill -auto -seed 7 # watch a whole campaign play itself
```

`-auto` plays without input, `-seed N` fixes who comes home, `-loss N` is one companion
lost in N bips. Two runs at the same seed must produce byte-identical output — story 28
requires determinism, so nothing may roll at render time.

## How to work here

**Read the relevant ADRs before proposing anything.** They are unusually specific about
what is *foreclosed*, and those lists are the point. A change that trips one is not a
judgement call — it needs the ADR superseded first. Several ADRs also have a **Worth
revisiting if** clause naming the exact symptom that would justify reopening them, and the
prescribed response, which is usually not the obvious one.

**Reasoning goes in code comments and ADRs, not in chat.** The house style is heavy doc
comments that say *why*, citing the ADR. Match it.

**Explain in plain language.** Dense answers full of ADR numbers, tables and file paths do
not land — say what was wrong, what changed, and what to run, in a few sentences. The ADR
reasoning belongs in the code comments where it is not forced on the reader. This was
learned the hard way; see the tuning logs for how much of it came from just playing the
thing.

**The prototype is for answering questions, not for polish.** It has already paid for
itself three times: it caught two decisions that were written down and never built
(ADR 0012's per-chapter ambient content, ADR 0006's familiarity legibility), and it moved
the campaign from sixteen chapters to twelve. When it says something is wrong, check
whether the harness is simply missing content before concluding the design is at fault —
that mistake was made once already and nearly cut the campaign on bad evidence.

**Nothing may surface a number at the player.** No familiarity meter, no chapter count, no
progress bar, no survivor tally, no timer, no visit allowance. This is four separate ADRs
agreeing, and it is the most common way a well-meant change breaks the design.

## Current state — 2026-07-25

Design is closed. Every open question in `spec.md` §11 is answered except the actual
tuning number, which needs an engine to test (see below).

- `campaign` module built and merged, exercised end to end by the prototype.
- Campaign is **twelve chapters**, loss rate **one bip in three** (ADR 0008 tuning log).
- Engine decided: **Unreal 5, PC first**, solo developer (ADR 0014).
- The encounter seam is designed: [ADR
  0015](docs/adr/0015-the-encounter-earns-survival-it-does-not-roll-it.md) — a companion
  carries a hidden per-bip tally of danger the player did not deny in time, resolved by the
  same skill-driven combat that resolves fire against the player, never a roll. Denying a
  threat means killing/suppressing it when the companion rides on the back, or never giving
  it the angle when the companion is on the back and the player rides. Familiarity (ADR
  0006) gates how much warning the player gets, not the threat itself.
  `campaign.BipOut.CompanionReturned bool` is unchanged — nothing downstream needs more
  than that, so the seam's shape was already right.
- No open PRs, no open issues, `main` green.

### The next decision

**Port `campaign` to C++** (ADR 0014 committed to porting rather than bridging — builds and
tests with `clang++` alone, no Unreal needed). The seam is now designed, so the port is no
longer at risk of baking in the wrong shape. After that, the encounter itself — the tally,
the denial mechanics, ADR 0015 describes but does not build — is Unreal-side work and needs
the engine.

### Picking this up on another machine

Neither Unreal nor a Go toolchain was found on the machine this ADR was written on — no
`UnrealEditor.exe`, no Epic Games Launcher, no `go` on `PATH` anywhere searched. Confirm
both are present before starting the C++ port or running `go test ./...`. Nothing is
half-finished — the tree is clean and every branch is merged, so there is no work-in-progress
to recover, only tooling to install.
