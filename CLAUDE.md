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

## Current state — 2026-10-01

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

- **The dirt bike and seat choice exist** (ADR 0004). `ADirtBike` is kinematic
  movement — accelerate, turn, drag — not Chaos Vehicle physics; a placeholder for
  realism, not for the seat structure, which is real: two fixed scene-component seats
  (`GetSeatComponent(EBikeSeat)`), matching how `campaign::BipOut.Companion` is one
  field rather than a collection. The driver's camera is fixed forward, not free-look —
  ADR 0004 gives the rider the approach and the escape, not aim, and that's what
  actually separates this seat from the passenger's. `AGrayBoxCharacter::AttachToBikeSeat`
  turns off its own movement and capsule collision and snaps onto the passenger seat,
  keeping look/fire untouched — proving those still work while the bike moves is the
  actual point of that seat. `AGrayBoxGameMode::bPlayerRides` is the seat choice
  itself, as a config toggle rather than a prep-screen menu that doesn't exist yet.
  Whichever seat the player doesn't take gets a placeholder occupant, deliberately not
  real companion AI — ADR 0004 names that as its own load-bearing, non-trivial work.
  Verified via a headless boot with `bPlayerRides = false` (the default at the time):
  player spawns attached to a placeholder-driven bike's passenger seat, zero errors.

- **The "exposure, not aim" denial mode exists for the riding seat** (ADR 0015), built
  and unit-tested, mechanics verified in the real level by the scripted drive, how it
  feels not yet judged (see below). When the player rides, the companion on the back
  shoots with fixed competence, so there's nothing to click — the player's only lever
  is route and timing. `AThreatActor` now has `EThreatDenial`:
  - `Aimed` — the original click-to-deny fuse, unchanged.
  - `Exposure` — the threat's clock only runs while it has a clear line of sight to
    the companion within `ExposureRange` (default 1500). It fires when the clock
    reaches `TimeToFire`. Breaking the sightline *resets* the clock to zero rather than
    pausing it, so ducking out of view even briefly buys a clean slate.

  The tally, threshold and `Fire()` are shared by both modes. The clock logic is
  `TickExposure(DeltaSeconds, bHasSightline)`, kept apart from the world trace in
  `Tick()` so it tests without a World — two new tests
  (`ExposureSustainedLongEnoughFires`, `ExposureBreakingSightlineResetsTheClock`)
  bring `OurBlock.Encounter` to six, all passing.

  **`bPlayerRides` now defaults to `true`.** In that mode the back seat gets
  `ACompanionStandIn` (so threats have a real tally to target), and
  `AGrayBoxGameMode::WireUnaimedExposureThreats` points any Exposure threat without a
  `Target` at it once it spawns — it can't be wired at level-edit time because it
  doesn't exist until runtime. `Content/Python/populate_exposure_test.py` places three
  Exposure threats along a driving line north of the origin cluster, one near the
  range cutoff. Flip `bPlayerRides` back to `false` and rebuild to return to the
  click-to-deny setup.

  **Known gap, since closed**: the floor started flat with no cover, so only distance
  could break a sightline — route couldn't. Cover now exists (see the cover entry
  below).

  **First play of the riding seat**: the bike now drives (W/S throttle and brake, A/D
  steer — steering only works while rolling, on purpose, and a human confirmed that
  feels right). The exposure threats' mechanics are now verified by the scripted drive
  (below), but how they feel hasn't been judged yet.

  **Gotcha hit getting it to drive** — another one with no error message pointing at
  the cause: on first play nothing responded at all, which looked like broken input.
  It wasn't. `ACompanionStandIn` kept its `BlockAllDynamic` collision after attaching
  to the back seat, and its cube overlaps the bike's body. **Unreal does not exempt an
  attached actor from its parent's movement sweep**, so `ADirtBike::Tick`'s swept move
  hit its own passenger every frame, zeroed `CurrentSpeed`, and — since steering needs
  speed — nothing moved or turned. Fixed by turning the stand-in's collision off on
  attach, the same as `AGrayBoxCharacter` already did with its capsule. Anything
  attached to the bike later (a real companion, props) needs the same treatment, or
  `IgnoreActorWhenMoving`. `ADirtBike` now logs `DirtBike: mapping context added`, so
  if input ever seems dead again, check the log first: that line present means input
  is fine and something is blocking movement.

- **A scripted drive checks the exposure mode without a human** (`AScriptedDrive`,
  `Encounter/ScriptedDrive.h`). It's a dev harness, not part of the game: launched with
  `-ScriptedDrive=<mode>`, it drives the bike along a fixed route through the bike's
  own held-key flags (so it runs exactly the movement code a player does), logs each
  exposure threat's distance and clock once a second, prints a `RESULT` line, and
  quits. Without the flag, nothing spawns it. Runs headless in seconds:

  ```
  UnrealEditor-Cmd.exe OurBlock.uproject /Game/Maps/TestLevel -game -nullrhi
      -unattended -nosplash -benchmark -fps=60 -ScriptedDrive=hide
  ```

  `-benchmark -fps=60` fixes the timestep so results don't depend on machine speed.
  The editor must be closed to rebuild it after changing its class layout (Live Coding
  can't add or reshape a class). Every mode follows the X=300 lane north — X=0 runs
  into the click-to-deny cluster's companion cube at the origin — by aiming at a point
  a fixed distance ahead along the lane; steering at discrete waypoints swung wide at
  full speed and clipped a wall. Each mode states the count it expects and the
  `RESULT` line says **PASS** or **FAIL**. A run that got stuck logs `BLOCKED` and can
  never pass, so a bike that silently sat still can't read as a clean result. The
  current modes, all passing:
  - `park` (never moves) — 0 fired; all three are out of range of the start.
  - `exposed` (stops at (300,600), in the open, in range of one threat) — that one
    fires after 5s, the other two don't.
  - `hide` (stops at (300,1900), in range of all three, behind cover from each) — 0
    fired, every clock stays at zero. This is the one that proves cover breaks a
    sightline.
  - `pass` (full speed straight through) — 0 fired.

  If a mode starts failing after the level changes, check the per-second status lines
  before blaming exposure mode — the level may simply have moved under the route.

  **What it found before there was cover — a design signal, not yet a decision**: on
  the flat floor an earlier `camp` mode (stop among them) had all three fire and the
  tally hit the threshold, while `pass` had two clocks reach 1.5s and reset on leaving
  range. At full speed the bike crosses a threat's whole range in about 2.5s, half its
  5s fuse, so on open ground keeping moving was always safe and only stopping was
  ever punished. That's what the cover below is for.

- **`TestLevel` has cover** (`Content/Python/add_cover.py`): three 300-tall walls, one
  between each exposure threat and part of the X=300 lane, laid out so the scripted
  drive can check them — (300,1900) is in range of all three threats but hidden from
  each, and (300,600) is in the open. 300 tall because threats sit at Z=100 and the
  companion rides at about Z=160; anything lower doesn't reliably break the trace. The
  walls block the bike too, which is the point: real cover is also something you can
  crash into. The script removes earlier `Cover*` actors before placing, so re-run it
  freely after tweaking positions. The layout is deliberately legible rather than
  clever — the walls only shape the lane; everything off it is open ground, there for
  a human to find out whether it feels like hiding.

  **Gotcha**: `-ExecutePythonScript` needs an **absolute** path. A relative one
  resolves against the engine's `Binaries/Win64`, not the project, and fails with
  "Could not load Python file" — while the editor process still exits 0. The older
  scripts' header comments show relative paths; they have the same problem.

  **Also noticed, not yet fixed**: `AThreatActor::Fire()` puts a red "ThreatActor_N
  fired" debug message on screen. It's gray-box debug output, but it tells the player
  something the design says they must never see — remove it once debugging is done.

### The next decision

**Play the riding seat, now that it has cover.** The scripted drives have settled
everything a script can: exposure mode works, and cover breaks a sightline. What's
left is the part only a human can judge — whether using the walls feels like hiding,
whether 5s/1500 units feels tense or trivial, and whether the player can tell
what's dangerous with no meter (none may ever be added). Expect three walls to be
thin; if it's inconclusive because there's too little to hide behind, add more cover
before retuning numbers. Rerun the four scripted drives after any level change. The other thread is unchanged: the "player on the back"
fuse/threshold still needs tuning by play (spec.md open question 2) — just flip
`bPlayerRides` back to do it. Don't tune both seats in the same session; one thing to
judge at a time.

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
