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
| `cpp/campaign/` | C++ port of the above (ADR 0014). Same seam, same story-numbered tests. |
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

```bash
# from cpp/campaign, see cpp/campaign/README.md for why -static matters here
clang++ -std=c++20 -Wall -Wextra -O0 -g -static \
    state.cpp cast.cpp apply.cpp outside.cpp flare.cpp persist.cpp campaign_test.cpp \
    -o campaign_test.exe
./campaign_test.exe
```

On Windows, `clang++` must be the **LLVM-MinGW** build specifically (self-contained
linker and runtime), not plain LLVM — plain LLVM's `clang++` defaults to the MSVC ABI
and cannot link without Visual Studio's linker and Windows SDK libs, which this machine
does not have. See "Toolchain" below.

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
- **`campaign` is ported to C++** (`cpp/campaign/`, ADR 0014). All 28 stories pass.
  Two of the Go suite's checks (stories 11 and 14, originally `reflect`-based) became
  `static_assert`s in `campaign.hpp` instead — stronger, since they fail the build
  rather than a test run. Everything else is a direct translation; see
  `cpp/campaign/README.md` for the handful of naming changes C++ needed that Go didn't.
- No open PRs, no open issues, `main` green.

### The next decision

**Build the encounter in Unreal.** The rules module now exists in both languages and
ADR 0015 describes what the encounter has to measure — a per-bip danger tally, denied
by the player's own skill, never a roll. What doesn't exist yet is Unreal itself: the
engine has never been installed or opened on any machine this project has run on. That's
the actual next step, not a design question — confirm the engine is present, then start
wiring `cpp/campaign`'s `Apply` seam to something that can produce a `CompanionReturned`
the way ADR 0015 describes, rather than roll for it.

### Toolchain — what's installed where this was last worked on

Confirm all of this is present before continuing on a new machine; none of it was here
when this session started, and installing it was itself part of the work.

- **Go** (`GoLang.Go` via `winget`) — for `campaign` and `cmd/roundhill`.
- **LLVM** (`LLVM.LLVM` via `winget`) — plain clang tools, but its `clang++` targets the
  MSVC ABI and **cannot link** without Visual Studio's linker/SDK, which this machine
  does not have. Don't use this one to build `cpp/campaign`.
- **LLVM-MinGW** (`MartinStorsjo.LLVM-MinGW.UCRT` via `winget`) — a second, separate
  `clang++` that's self-contained (own linker, own runtime). This is the one
  `cpp/campaign` actually builds with. It's a large archive and extraction is slow
  (likely real-time antivirus scanning many small files) — budget several minutes, not
  seconds, and don't mistake the wait for a hang.
- **Unreal** — still not installed anywhere. Needed for the actual next step above.

Neither Bash nor PowerShell in this environment picks up a `winget`-installed program's
new `PATH` entry automatically mid-session; each tool call inherits whatever `PATH` was
current when its shell started. Prefix commands with the install directory (or, for a
one-off interactive shell, `export PATH="$PATH:/c/Program Files/Go/bin:..."`) rather than
assuming a fresh call will see it.
