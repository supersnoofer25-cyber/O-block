# campaign (C++)

Port of [`../../campaign`](../../campaign) per [ADR
0014](../../docs/adr/0014-unreal-5-on-pc.md): the Go module ports to C++ rather than
being bridged into Unreal, because it was built headless, pure, and dependency-free
precisely so it could outlive the engine decision.

No engine is needed to build or test this. Nothing here renders, reads input, or
depends on anything but the C++ standard library — the same "no engine" scope as the Go
source, kept exactly.

## Building and testing

ADR 0014 says this builds and tests with `clang++` alone. On Windows that specifically
means a **self-contained** clang toolchain — plain LLVM's `clang++` depends on the MSVC
linker and Windows SDK libraries it does not ship, so it cannot link without Visual
Studio installed. [LLVM-MinGW](https://github.com/mstorsjo/llvm-mingw) is still
`clang++`, just bundled with its own linker and runtime, and needs nothing from Visual
Studio.

```
clang++ -std=c++20 -Wall -Wextra -O0 -g -static \
    state.cpp cast.cpp apply.cpp outside.cpp flare.cpp persist.cpp campaign_test.cpp \
    -o campaign_test.exe
./campaign_test.exe
```

`-static` matters on Windows specifically: a dynamically-linked binary from this
toolchain depends on UCRT DLLs (`api-ms-win-crt-*.dll`) that aren't guaranteed present,
and weren't on the machine this was first built on. Static linking bundles the runtime
into the executable instead of assuming the target machine has it, which is one fewer
thing this port depends on.

Every test prints `PASS` or `FAIL` with the story it's named for; the run ends with a
`passed / failed / total` line and a non-zero exit code on any failure.

## The seam

One function, same as Go.

```cpp
Result Apply(const State& s, const Decision& d);
```

Current state in, one player decision in, a new state and an error out — Go's
`(State, error)` has no direct C++ equivalent this cheap, so `Result` carries both and
`Result::Ok()` mirrors `if err != nil`. There are exactly two decisions
(`SpendEvening`, `BipOut`), which is the whole choice architecture
([ADR 0003](../../docs/adr/0003-choice-lives-in-systems-not-dialogue.md)), expressed as
a `std::variant<SpendEvening, BipOut>` rather than a Go interface — a closed sum type
either way, with the compiler able to check it's handled exhaustively.

**The encounter is not simulated here**, same as Go. Its outcome arrives as
`CompanionReturned` — the module consumes results and never produces them (story 15).
[ADR 0015](../../docs/adr/0015-the-encounter-earns-survival-it-does-not-roll-it.md)
settles how the encounter itself earns that bool; nothing about this seam changed as a
result, in either language.

## Where this differs from the Go source, and why

**Two run-time checks became compile-time ones.** Go's story 11 and story 14 tests used
`reflect` to check, at test time, that `BipOut.Companion` can't be a collection and that
`Member` carries no numeric field. C++ can enforce both with `static_assert` next to the
types themselves, in `campaign.hpp` — a violation fails the build rather than a test
run, which is strictly stronger than what the Go version could do. The ported test file
still carries a test named for each story, but the real enforcement moved to the header.

**A few fields were renamed to dodge a footgun Go doesn't have.** Go can write
`Capability Capability` because a field and its type live in separate namespaces; C++
puts them in the same one, and while a member declaration can legally shadow an
enclosing type name the moment it's declared, doing that for every field read like an
invitation to a confusing bug later. `Member.Capability` became `Member.Cap`;
`State.LastBip`'s type became `LastBipInfo` instead of a lowercase `lastBip`, since C++
has no case-based visibility distinction to lean on the way Go's exported/unexported
convention does. `BipOut.Seat` kept its name — it was tested to compile cleanly, and
renaming it would have cost more legibility (call sites stop reading like the Go
source) than it bought in safety.

**JSON has no standard-library equivalent.** `persist.cpp` carries a JSON value type
and a parser/writer sized for exactly one job — round-tripping `State` — rather than
taking on a dependency. It supports less of the JSON spec than a general library would
(no `\u` escapes, for instance) because it only ever has to read what `Save` just wrote.

## Deliberately not here

Same list as the Go module's own, for the same reasons — see
[`../../campaign/README.md`](../../campaign/README.md#deliberately-not-here). Flares,
tie ripples, and cost-of-loss effects are writing and engine work, not campaign state.
Encounter feel, bike handling, hub traversal, and companion AI are judged by playing,
which is Unreal-side work this port does not attempt.
