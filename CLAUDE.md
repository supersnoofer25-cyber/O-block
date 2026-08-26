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
| `OurBlock/` | The Unreal 5.8 project. `Source/OurBlock/Campaign/` wires `cpp/campaign` in. |

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

## Current state — 2026-08-26

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
- **`OurBlock/` builds and opens.** `Build.bat OurBlockEditor Win64 Development
  -Project=OurBlock/OurBlock.uproject` produces `UnrealEditor-OurBlock.dll` against MSVC
  14.44 and the Windows 10 SDK. Required installing Visual Studio Build Tools 2022 with
  the C++ workload — Unreal's own build tooling needs the MSVC ABI, the same constraint
  that ruled out plain LLVM for `cpp/campaign`, and there's no MinGW workaround for the
  engine itself. The editor has since been launched against the project directly
  (`UnrealEditor.exe OurBlock.uproject`) and opened clean: asset registry scan completed,
  default map passed `MapCheck` with 0 errors/0 warnings, no missing-module dialog. The
  hand-written skeleton is now fully verified — built, opened, and running.

- **The `Apply` seam is wired into `OurBlock`.** `Source/OurBlock/Campaign/` holds
  one-line shims (`#include "../../../../cpp/campaign/*.cpp"`) that compile
  `cpp/campaign`'s exact source directly into the `OurBlock` module — no separate
  Unreal module, on purpose: an Editor build is modular (every module is its own DLL),
  and `cpp/campaign` has no dllexport/dllimport decoration since it was built to know
  nothing about UE. A separate module would need that decoration added just to cross
  its own DLL boundary, which means touching the one file this project keeps as its
  tested, standalone specification purely to satisfy a Windows linking mechanism.
  Compiling the shims into the same DLL that calls them sidesteps the problem — there's
  no boundary to cross. `OurBlock.Build.cs` needed `CppStandard = Cpp20` and
  `bEnableExceptions = true` to match what the standalone `clang++` build already
  assumed. `CampaignSubsystem` (a `UGameInstanceSubsystem`) owns a `campaign::State`,
  exposes `SpendEvening`/`BipOut` to Blueprint via `ECampaignSeat`/`ECampaignError`
  (`CampaignTypes.h`), and is the one place `FName` and `std::string` meet — nothing
  behind `Campaign.h` ever sees an `FName`. Verified with a headless automation test
  (`CampaignPortTest.cpp`, `OurBlock.Campaign.PortCompilesAndRuns`) run via
  `UnrealEditor-Cmd.exe OurBlock.uproject -ExecCmds="Automation RunTests
  OurBlock.Campaign; Quit" -unattended -nopause -nullrhi` — passed, proving the ported
  rules produce correct results compiled by MSVC under UE's settings, not just under
  the `clang++` build that's the actual specification.
- **`UCampaignSubsystem` is confirmed running for real**, not just at the C++ level.
  `Content/Maps/TestLevel.umap` is a flat plane and a `PlayerStart`, nothing else —
  created by `Content/Python/create_test_level.py`, kept as a reproducible script
  rather than undocumented manual editor clicking. Launching the actual game against it
  (`UnrealEditor-Cmd.exe OurBlock.uproject /Game/Maps/TestLevel -game -nullrhi
  -unattended -ExecCmds="quit"`) logs `UCampaignSubsystem initialized: 8 alive, chapter
  1` and loads the map clean, with no errors anywhere in the run. That closes the gap
  the automation test couldn't reach: a `UGameInstanceSubsystem` only initializes once a
  real `UGameInstance` exists, which needs a level, which now exists.

  **Gotcha hit along the way**: Git Bash mangles a bare `/Game/...` engine content path
  into a Windows filesystem path before Unreal ever sees it (MSYS argv conversion).
  Prefix the command with `MSYS2_ARG_CONV_EXCL="*"` when passing one from this shell.

- **A gray-box of ADR 0015's danger tally exists and is playable.**
  `Source/OurBlock/Encounter/`:
  - `UDangerTallyComponent` — the tally and threshold themselves. `AddDanger` is called
    only when a threat wasn't denied in time; `DidCompanionSurvive` is a plain
    threshold check, never a roll.
  - `AThreatActor` — a sphere with a countdown. `Deny()` (called by the player's fire
    trace) cancels it and adds nothing; left alone, it fires and adds
    `DangerOnFire` to whatever `UDangerTallyComponent` it targets. Firing is final —
    denying a threat after it already fired does not undo the danger it added.
  - `ACompanionStandIn` — a cube holding the tally component, nothing else. Not a
    companion in ADR 0006/0009's sense; just something for threats to be aimed at.
  - `AGrayBoxCharacter` / `AGrayBoxGameMode` — WASD, mouselook, left-click fires a line
    trace that calls `Deny()` on whatever `AThreatActor` it hits. Input is built at
    runtime in C++ (`NewObject<UInputMappingContext>`, mapped to engine `FKey`s
    directly) rather than loaded from a Content asset, so none of this needed the
    editor GUI to author.

  The tally/threat logic is verified headlessly (`EncounterPortTest.cpp`,
  `OurBlock.Encounter.*`, four tests — denial adds nothing, firing adds exactly its
  weight, firing is irreversible, outcome is a deterministic threshold not a roll) the
  same way as `OurBlock.Campaign`'s suite. `Content/Maps/TestLevel.umap` has
  `ACompanionStandIn` near the origin and three `AThreatActor`s around it
  (`Content/Python/populate_test_encounter.py`), a `PlayerStart` well clear of them
  facing the group, a movable sun and sky light, and `AGrayBoxHUD` drawing a plain
  crosshair. Open the project and press Play, or run `UnrealEditor.exe
  OurBlock.uproject` and Play-In-Editor on `TestLevel`.

  **A human has actually played it and it works.** Aim the crosshair at a sphere and
  click before its fuse runs out — denied, it vanishes for nothing; missed, it fires
  and the tally moves, invisibly, exactly as ADR 0015 specifies. First playtest
  feedback: 3s felt too fast to react to, 10s (only used to isolate bugs, see below)
  felt like no pressure at all. Currently sitting at 5s
  (`Content/Python/set_fuse_timing.py`) as a first real data point, not a final answer —
  `AThreatActor::TimeToFire` and `UDangerTallyComponent::Threshold` are both still
  placeholders meant to keep moving until the timing actually feels tense rather than
  either trivial or unfair.

  **Gotchas hit getting from "compiles" to "actually playable"** — worth knowing before
  touching this code again, since none of them produced an error message that pointed
  at the actual cause:
  - A level's own World Settings → GameMode Override takes priority over
    `DefaultEngine.ini`'s `GlobalDefaultGameMode` unconditionally. `TestLevel` predated
    `AGrayBoxGameMode`, so it silently ran bare `GameModeBase` (no pawn, no input) even
    with the ini set correctly, until the level's own override was set directly
    (`Content/Python/set_test_level_gamemode.py`).
  - `TestLevel` needs its own light — Lumen GI with zero lights in the level renders as
    an effectively black scene.
  - `AGameModeBase::RestartPlayer` spawns the pawn (running `BeginPlay`) *before*
    calling `Possess()` on it — and separately, `SetupPlayerInputComponent` (called
    during possession) also runs *before* `BeginPlay`. An `EnhancedInputComponent`
    `BindAction` against a still-null `UInputAction` silently binds nothing, so input
    actions have to exist by construction time, not `BeginPlay`, or controls look
    wired up but do nothing with no error anywhere.
  - Inside an `AActor` constructor, `NewObject<T>(this)` for anything meant to persist
    (like those input actions) hard-crashes — the engine requires
    `CreateDefaultSubobject<T>(Name)` instead, the sanctioned way to create *any*
    `UObject` subobject from a constructor, not just components.
  - `UCameraComponent` doesn't follow the controller's pitch by default, only whatever
    rotation it inherits from its attach parent — `bUsePawnControlRotation = true` is
    what makes vertical mouselook actually move the camera.
  - `unreal.Rotator`'s Python constructor takes positional args as **(Roll, Pitch,
    Yaw)**, not `(Pitch, Yaw, Roll)`. Confirmed by reading a saved value back rather
    than assuming — `Rotator(0, 90, 0)` intended as yaw silently became pitch, pointing
    a spawned actor straight up instead of at anything.

### The next decision

**Keep tuning the fuse and threshold by playing, then decide if the mechanic is worth
building for real.** The gray-box works end to end and one round of feedback has
already moved the fuse from 3s to 5s; more playthroughs at different `TimeToFire` /
`Threshold` values are what actually answers spec.md's open question 2, not more code.
Once the timing feels right, replacing the gray-box actors with real ones (bike, seat,
companion AI, real weapons) is production work, not a design question — ADR 0015
already settled what the mechanism is, this only tests whether it's the right one.

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
- **Unreal 5.8** — installed via the Epic Games Launcher at `E:\Epic Games\UE_5.8`
  (`UnrealEditor.exe` under `Engine\Binaries\Win64`). The launcher itself lives at
  `D:\Epic Games\Launcher`. Both are on different drives than the OS and this repo —
  don't assume `C:` when looking for either. `OurBlock/` builds and opens; the project
  targets Shader Model 6 (Project Settings > Platforms > Windows), which needed the VC++
  redistributable bundled at `Engine/Extras/Redist/en-us/vc_redist.x64.exe` updated to
  clear an "outdated" warning on launch.
- **A build with the editor open needs Live Coding (Ctrl+Alt+F11), not `Build.bat`** —
  `Build.bat` refuses to run while Live Coding holds the module, and Live Coding itself
  can't link in a module that didn't exist when the editor launched (no prior `.lib` to
  patch). Adding a brand-new module means closing the editor, running `Build.bat`, then
  reopening — only patches to modules that already existed at launch can go through
  Live Coding.
- **Visual Studio Build Tools 2022** (`Microsoft.VisualStudio.2022.BuildTools` via
  `winget`, with the `Microsoft.VisualStudio.Workload.VCTools` override) — Unreal's
  build tooling requires the MSVC ABI on Windows; there is no LLVM-MinGW-style
  workaround for the engine itself the way there was for `cpp/campaign`. First two
  `winget install` attempts both failed with exit 1602 ("cancelled") when run through a
  non-interactive shell — same failure mode LLVM-MinGW hit — and it only succeeded once
  run directly in an interactive shell where the UAC prompt could be approved.
- **VS Code** (`Microsoft.VisualStudioCode` via `winget`) — installed to
  `%LOCALAPPDATA%\Programs\Microsoft VS Code\Code.exe`, for editing `cpp/campaign` and
  the Unreal C++ modules.

Neither Bash nor PowerShell in this environment picks up a `winget`-installed program's
new `PATH` entry automatically mid-session; each tool call inherits whatever `PATH` was
current when its shell started. Prefix commands with the install directory (or, for a
one-off interactive shell, `export PATH="$PATH:/c/Program Files/Go/bin:..."`) rather than
assuming a fresh call will see it.
