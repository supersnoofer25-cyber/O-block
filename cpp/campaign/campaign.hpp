// The headless, engine-agnostic rules module for Our Block, ported from the Go module
// of the same name (../../campaign) per ADR 0014: Unreal is C++, and the module was
// built headless, pure, and dependency-free precisely so it could outlive the engine
// decision. This header is that promise kept — translated, not redesigned.
//
// It owns what the design commits to and nothing else: the set and who is alive, how
// time outside was spent, which seat Jamal took and who rode with him, and how the
// cycle escalates. It does not know what a gunfight is — an encounter is resolved
// elsewhere (ADR 0015) and hands the module a result, which the module makes permanent.
//
// The interface is one pure function, Apply: current state in, a player decision in,
// new state out. Nothing here renders, reads input, or depends on anything but the
// C++ standard library — matching the Go module's own "no engine" scope exactly, and
// keeping the promise that started this file: build and test with a compiler alone.
//
// Vocabulary is CONTEXT.md's and is used strictly. Reasoning lives in docs/adr, and
// where a translation choice needed its own reasoning that the Go source didn't, it is
// recorded here rather than in chat.
//
// A note on naming: several Go fields share a name with their own type
// (`Capability Capability`, `Seat Seat`) — legal there because Go's field and type
// namespaces never collide. C++'s do, though a member declaration may still shadow an
// enclosing type name the moment it is declared, which is what makes `Seat Seat;`
// compile below. Where that shadowing would read as a mistake rather than a
// translation, the C++ name was adjusted instead (`Capability` the struct became `Cap`
// the field); where it stayed exactly as legible as the Go, it was kept.
#pragma once

#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace campaign {

// MemberID identifies one member of the set.
//
// Deliberately a string rather than an index, exactly as in Go: an identifier is not a
// number, and nothing on a member may be numeric (ADR 0009). Keeping IDs non-numeric
// also keeps them out of arithmetic and comparison by accident.
using MemberID = std::string;

// The eight, as string constants rather than an enum. An enum would reintroduce the
// numeric index ADR 0009 forecloses and would have nowhere to put an unrecognised
// value like the invented companion story 10 tests for — a plain string does both for
// free, the same reason Go chose it.
inline const MemberID Alex = "alex";
inline const MemberID Tyrone = "tyrone";
inline const MemberID Anthony = "anthony";
inline const MemberID James = "james";
inline const MemberID Daquavion = "daquavion";
inline const MemberID Marcus = "marcus";
inline const MemberID Omar = "omar";
inline const MemberID Joshua = "joshua";

// Tuning values. Named and gathered because they are tuning rather than decision —
// spec.md open question 2 is explicit that whether the encounters as built hit the
// target can only be answered by playing them. Values match state.go exactly; the
// reasoning behind each lives there and in ADR 0008/0013 and is not repeated here.
inline constexpr int Chapters = 12;
inline constexpr int IntensityCeiling = 4;
inline constexpr int DepartAlongside = 3;
inline constexpr int EveningsToClose = 5;

// Level is how well Jamal knows someone, in words rather than as a number.
//
// The underlying count is never surfaced: ADR 0006 forecloses familiarity as a meter
// or tier the player can read, and ADR 0012 forecloses anything countable outside.
// This exists so the rules can talk about it, not so a UI can draw it.
enum class Level { Stranger, Acquainted, Known, Close };

// Seat is where Jamal sits on a given bip (ADR 0004). Chosen fresh every time.
//
// None is the zero value, standing in for Go's empty string — a BipOut with no seat
// set is invalid the same way it is in Go, and Apply rejects it the same way.
enum class Seat { None, Riding, OnBack };

// EndReason is why a campaign stopped.
//
// Neither non-zero value is a failure, and there is deliberately no third value that
// would be. ADR 0002 forbids presenting a set member's death as a losing outcome, and
// the cheapest way to guarantee that here, as in Go, is to give the type nowhere to
// put one.
enum class EndReason {
  NotEnded,  // the zero value, so a fresh campaign is running by default
  Finished,  // the campaign reaching its last chapter (ADR 0007)
  SetEmpty,  // the same ending, reached early because nobody is left to fill the seat
};

// Capability is what a member is like on a bip.
//
// A behaviour, never a rating, and defined in both seats — a capability that existed
// in only one seat would collapse the seat choice into the companion choice, which is
// the failure ADR 0004 names. There is no field here that could order two members.
//
// Unread and Unprompted are the same capability seen from further away and from closer
// in. They are not extra capabilities and not a second axis: what changes between them
// is how much of the one behaviour Jamal can make out, which is exactly and only what
// ADR 0006 lets familiarity change. Nobody gets better.
struct Capability {
  std::string Name;    // what he does, in a phrase
  std::string Riding;  // how it shows when he has the bike
  std::string OnBack;  // how it shows when he has the gun

  // Unread is what it is like to go out with him without knowing him. Written as
  // Jamal's failure to read the man, never as the man being worse — a line that
  // implied he performs badly for a stranger would be the competence delta ADR 0006
  // rejects, arriving as prose instead of as a number.
  std::string Unread;

  // Unprompted is what he tells Jamal without being asked, once Jamal knows him.
  // ADR 0006 names this as where familiarity shows: information, which the player
  // still has to do something with.
  std::string Unprompted;
};

// Member is one of the eight. Immutable for the whole campaign — only the living
// roster and familiarity change (ADR 0009).
//
// Every field but ID and Tie is plain text. That mirrors story 14 (TestStory14 in the
// Go suite, ported below): with nothing numeric to compare, the roster cannot be
// ranked, which is what ADR 0003 put the game's whole choice architecture on one
// screen to prevent.
struct Member {
  MemberID ID;
  std::string Name;
  Capability Cap;          // Go: Member.Capability — renamed to avoid shadowing the type
  std::string CostOfLoss;  // what the Block loses when he is killed
  std::string Activity;    // where you find him, as a phrase that slots into a sentence
  MemberID Tie;            // the one other member he is tied to
};

// Story 14, enforced at compile time rather than by the run-time reflection Go used:
// every field on Capability and Member is a string, so two members can never be
// compared and the roster can never become a ranking (ADR 0003). A field added here
// with a numeric type fails the build, not a test run.
static_assert(std::is_same_v<decltype(Capability::Name), std::string>);
static_assert(std::is_same_v<decltype(Capability::Riding), std::string>);
static_assert(std::is_same_v<decltype(Capability::OnBack), std::string>);
static_assert(std::is_same_v<decltype(Capability::Unread), std::string>);
static_assert(std::is_same_v<decltype(Capability::Unprompted), std::string>);
static_assert(std::is_same_v<decltype(Member::ID), MemberID>);
static_assert(std::is_same_v<decltype(Member::Name), std::string>);
static_assert(std::is_same_v<decltype(Member::CostOfLoss), std::string>);
static_assert(std::is_same_v<decltype(Member::Activity), std::string>);
static_assert(std::is_same_v<decltype(Member::Tie), MemberID>);

// TheSet is the roster at campaign start, in a fixed order.
//
// A std::vector rather than any associative container, so iteration order is stable
// and story 28's determinism cannot be broken by an unordered container's iteration
// order — the same reason Go used a slice and not a map.
//
// There is deliberately no function that appends to this or builds a different set:
// story 13 forecloses recruitment and replacement, and the cheapest way to guarantee
// that is to give the module no function that could do it.
extern const std::vector<Member> TheSet;

// MemberByID returns the immutable definition of a member, or false if id names
// nobody — which happens whenever a caller hands Apply a string that is not one of
// the eight (story 10).
bool MemberByID(const MemberID& id, Member& out);

// Familiarity is the count of evenings Jamal has spent with one member.
struct FamiliarityEntry {
  MemberID Member;
  int Evenings = 0;
};

// LastBipInfo is what happened the last time Jamal went out.
//
// Tonight's reason to go comes from this and from nothing else (ADR 0011), which is
// why the module has to remember it: with every external flare generator foreclosed —
// no schedule, no escalation manager, no quest-giver — the previous bip is the only
// thing left that could cause the next one. Its default-constructed value means the
// campaign has not had a bip yet, and chapter one needs no special case because the
// beef is inherited.
struct LastBipInfo {
  bool Happened = false;
  bool Lost = false;
  MemberID Who;
};

// State is the whole campaign. Every collection is a std::vector in a stable order so
// that identical decisions reproduce identical state (story 28).
struct State {
  int Chapter = 1;  // 1-based; Chapters is the last

  // Alive is the living roster, in TheSet's order. It only ever shortens
  // (stories 22, 23).
  std::vector<MemberID> Alive;

  // Outside is who is still out this evening. Empty means the only thing left to do
  // is go (ADR 0012).
  std::vector<MemberID> Outside;

  // Familiarity is per member and never decreases (stories 3, 5).
  std::vector<FamiliarityEntry> Familiarity;

  // Cycle counts completed bips. It advances on every bip regardless of outcome
  // (stories 16, 17) and nothing reduces it (story 18).
  int Cycle = 0;

  // LastBip is what the next flare is made of (ADR 0011). Go: State.LastBip of type
  // lastBip — renamed to LastBipInfo here for the same reason Capability was renamed.
  LastBipInfo LastBip;

  EndReason End = EndReason::NotEnded;

  // Running reports whether the campaign is still going.
  bool Running() const { return End == EndReason::NotEnded; }

  // IsAlive reports whether a member is still alive.
  bool IsAlive(const MemberID& id) const;

  // IsOutside reports whether a member is still out this evening.
  bool IsOutside(const MemberID& id) const;

  // Evenings is how many evenings Jamal has spent with a member.
  int Evenings(const MemberID& id) const;

  // Knows is how well Jamal knows a member, in words.
  Level Knows(const MemberID& id) const;

  // Intensity is how bad things are now, which is not the same as how far the cycle
  // has run. It climbs with the cycle and then stops (ADR 0013).
  //
  // The two are separate on purpose. Stories 16 and 17 require every bip to escalate
  // the cycle regardless of outcome, and ADR 0013 requires the intensity the player
  // meets to plateau. A single counter cannot satisfy both; Cycle always advances,
  // Intensity caps.
  int Intensity() const;

  // Reading is what Jamal can make out about a companion before a bip, given how well
  // he knows him and which seat he is taking.
  //
  // This is the whole of what familiarity does (ADR 0006). It supplies information and
  // never competence: the same man performs identically whether Jamal has spent five
  // evenings with him or none, and nothing here is reachable from Apply — a reading
  // cannot change an outcome, only what the player knew going in.
  std::string Reading(const MemberID& id, Seat jamalSeat) const;

  // Companions are the members who could fill the other seat right now: the living,
  // in a stable order (story 12).
  std::vector<MemberID> Companions() const { return Alive; }

  // SetEvenings is the only writer of familiarity, and it never lowers a value
  // (story 5). Public because C++ has no equivalent of "unexported within the same
  // package" — Go's setEvenings is exactly as reachable from every other file in
  // package campaign as this is from every other file here. Apply is the only caller.
  void SetEvenings(const MemberID& id, int n);

  // Clone deep-copies a state so Apply can stay a pure function. Go: State.clone.
  State Clone() const { return *this; }  // std::vector's copy ctor does the deep copy
};

// New starts a campaign: chapter one, all eight alive and outside.
//
// Joshua begins at Close. You cannot not know your own brother — he is Jamal's
// younger brother and lives with him (docs/cast.md). This is not the special case the
// cast contract forbids: the contract governs which properties a member carries, and
// every member carries the same ones. A starting value is not a property.
State New();

// The errors Apply can return. Every one is a decision that could not be taken — none
// is a campaign outcome. Losing a member is not among them, because a death is not a
// failure (story 24, ADR 0002).
//
// Go returns these as an `error` interface with sentinel values; C++ has no exception-
// free equivalent that is this cheap, so Apply returns a (State, Error) pair the same
// shape as Go's (State, error) — see Result below.
enum class Error {
  None,        // no error: the decision was legal
  Ended,       // campaign has ended
  NotAMember,  // not a member of the set
  Dead,        // member is dead
  GoneInside,  // member has gone inside for the night
  Seat,        // seat must be riding or on-the-back
};

// Decision is something the player does. There are exactly two, which is the whole
// choice architecture (ADR 0003): who to spend an evening with, and going out — with
// which seat and which companion.
//
// SpendEvening is time outside with one member: under a car with Marcus, at the hoop
// with Omar. Not a conversation — doing the thing he does, where he does it.
struct SpendEvening {
  MemberID With;
};

// BipOut is going out, and it ends the chapter.
//
// The encounter is not simulated here. Its outcome arrives as CompanionReturned — the
// module consumes results and never produces them (story 15). Whether a man comes home
// is decided by play (ADR 0001), earned the way ADR 0015 describes, which is what makes
// the promise true.
struct BipOut {
  Seat Seat = Seat::None;
  MemberID Companion;
  bool CompanionReturned = false;
};

// Story 11, enforced at compile time: Companion is a single MemberID, never a
// collection, so a second passenger cannot be expressed. There is no number to raise
// and no rule to relax — the cap is structural, the same as in Go.
static_assert(std::is_same_v<decltype(BipOut::Companion), MemberID>);

// A closed sum type: Decision is exactly SpendEvening or BipOut and nothing else,
// which is the same guarantee Go got from an unexported interface method — no caller
// outside this file can invent a third decision. std::variant was chosen over a small
// class hierarchy because Apply's switch-on-type reads as a direct translation of
// Go's type switch, with the compiler enforcing exhaustiveness the same way Go's
// default case had to be trusted to.
using Decision = std::variant<SpendEvening, BipOut>;

// Result is Apply's return: the same (State, error) shape Go uses, since C++17 has no
// std::expected. Ok() mirrors the Go idiom `if err != nil`.
struct Result {
  State S;
  Error Err = Error::None;
  bool Ok() const { return Err == Error::None; }
};

// Apply is the seam: current state in, one decision in, new state out.
//
// Pure — it never mutates its argument and depends on nothing but its inputs, which is
// what makes story 28 hold. Identical decision sequences reproduce identical state
// because there is nothing else for the result to depend on.
Result Apply(const State& s, const Decision& d);

// A flare is why tonight.
//
// ADR 0011 generates it from the player's previous bip and forecloses every external
// source — no schedule, no escalation manager, no quest-giver. There is no system
// here, only a lookup from what the player did last time to what the Block says about
// it now.
struct Flare {
  // News is how it reaches the player: outside, from people, in the wrong order and
  // more than once. Never a marker, an objective or a briefing.
  std::string News;

  // Dead is somebody on the Block who was killed, or empty. A flare does not kill
  // every time.
  std::string Dead;
};

// FlareFor is tonight's reason, read off the last bip. Pure and deterministic, like
// Apply and for the same reason.
Flare FlareFor(const State& s);

// Tonight is where a member is this evening, in a phrase that slots in after his name.
// The chapter selects the line, and the campaign never shows the player a chapter
// number (ADR 0008) — this is the number being used to author against, not to display.
std::string Tonight(const MemberID& id, int chapter);

// WentIn is why a member went inside, as a clause following his name.
std::string WentIn(const MemberID& id, int chapter);

// SlotName is the one save file. There is exactly one, and it is overwritten.
//
// ADR 0002 makes checkpointing autosave-only, because manual saves make save-scumming
// trivial and therefore make permanence optional. This module offers no way to name a
// slot, list slots, or restore an earlier one — story 27 is enforced by the absence of
// an API rather than by a rule someone has to remember.
inline constexpr const char* SlotName = "campaign.json";

// Save writes the campaign to its single slot, replacing what was there. The write is
// atomic via a temporary file and rename, so a crash mid-write cannot leave a
// half-written campaign.
//
// Returns true on success. Go signals failure with an error value carrying detail;
// this module only ever fails on a filesystem it cannot see making a directory or
// writing a file, and nothing in the ported test suite inspects the message, so a
// bool is the whole translation needs.
bool Save(const std::string& dir, const State& s);

// Load reads the campaign back, resuming exactly where it stopped (story 26). Returns
// true on success, leaving out untouched on failure.
bool Load(const std::string& dir, State& out);

}  // namespace campaign
