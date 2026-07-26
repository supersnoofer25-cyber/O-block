// Port of campaign/campaign_test.go. One test per [seam] story in issue #3, same as
// the Go suite, named so a failure names the story it broke. The [playtest] stories
// are deliberately absent here too — encounter feel, bike handling, hub traversal and
// companion AI are judged by playing.
//
// This file is also the answer to ADR 0014's "builds and tests with clang++ alone": no
// GoogleTest, no Catch2, nothing beyond the standard library and a couple of hundred
// lines of test runner, exactly matching the module's own no-dependencies rule.
//
// Two stories (11, 14) were run-time reflection checks in Go. C++ can enforce the same
// guarantees at compile time instead, via static_assert next to the types in
// campaign.hpp — stronger than the Go version, since a violation there fails the build
// rather than a test run. This file still carries a test for each so the one-test-
// per-story mapping holds, but the real enforcement is the static_assert.
#include "campaign.hpp"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <type_traits>

namespace campaign {
// Defined in state.cpp; not part of the public header, same as Go's unexported
// contains(). Forward-declared here so the story 1 and 13 checks below can use it.
bool Contains(const std::vector<MemberID>& ids, const MemberID& id);
}  // namespace campaign

namespace campaign::test {
namespace {

// --- a test runner sized for this file and nothing else --------------------------

struct Failure {
  std::string msg;
};

void Fail(const std::string& msg) { throw Failure{msg}; }

// Require aborts the current test immediately, matching t.Fatalf.
#define REQUIRE(cond, msg)      \
  do {                          \
    if (!(cond)) Fail(msg);     \
  } while (0)

// Check records a failure but lets the test keep running, matching t.Errorf. Since
// this runner has no per-test accumulator, Check is REQUIRE's equivalent here too —
// the ported stories that used t.Errorf never depended on continuing past the first
// mismatch to report a second one, so nothing is lost by making both fatal.
#define CHECK(cond, msg) REQUIRE(cond, msg)

struct TestCase {
  std::string name;
  void (*fn)();
};

std::vector<TestCase>& Registry() {
  static std::vector<TestCase> r;
  return r;
}

struct Registrar {
  Registrar(const char* name, void (*fn)()) { Registry().push_back({name, fn}); }
};

#define TEST(name)                                     \
  static void Test_##name();                           \
  static ::campaign::test::Registrar reg_##name(#name, Test_##name); \
  static void Test_##name()

// --- helpers mirroring campaign_test.go's ----------------------------------------

// Must(Apply(...)) asserts a decision was legal and returns the new state, matching
// Go's must(t)(Apply(...)).
State Must(Result r) {
  REQUIRE(r.Ok(), "unexpected error from Apply");
  return r.S;
}

// SpendAll takes every evening the chapter will give, then goes out — Go's spendAll.
State SpendAll(State s, bool companionReturns) {
  while (!s.Outside.empty()) {
    s = Must(Apply(s, SpendEvening{s.Outside[0]}));
  }
  BipOut bip;
  bip.Seat = campaign::Seat::Riding;
  bip.Companion = s.Alive[0];
  bip.CompanionReturned = companionReturns;
  return Must(Apply(s, bip));
}

// StatesEqual mirrors reflect.DeepEqual(a, b) for the one type the Go suite ever
// compares that way. State has no operator== of its own — adding one purely so a test
// file could use it would be scope the module itself has no need for — so this
// compares every field by hand instead, shared by stories 26 and 28.
bool StatesEqual(const State& a, const State& b) {
  if (a.Chapter != b.Chapter || a.Cycle != b.Cycle || a.End != b.End) return false;
  if (a.Alive != b.Alive || a.Outside != b.Outside) return false;
  if (a.LastBip.Happened != b.LastBip.Happened || a.LastBip.Lost != b.LastBip.Lost ||
      a.LastBip.Who != b.LastBip.Who)
    return false;
  if (a.Familiarity.size() != b.Familiarity.size()) return false;
  for (size_t i = 0; i < a.Familiarity.size(); ++i) {
    if (a.Familiarity[i].Member != b.Familiarity[i].Member ||
        a.Familiarity[i].Evenings != b.Familiarity[i].Evenings)
      return false;
  }
  return true;
}

// TempDir mirrors t.TempDir(): a fresh directory, removed when the guard goes out of
// scope.
class TempDir {
 public:
  TempDir() {
    static std::atomic<long long> counter{0};
    auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    path_ = std::filesystem::temp_directory_path() /
            ("oblock_campaign_test_" + std::to_string(now) + "_" +
             std::to_string(counter++));
    std::filesystem::create_directories(path_);
  }
  ~TempDir() {
    std::error_code ec;
    std::filesystem::remove_all(path_, ec);
  }
  std::string String() const { return path_.string(); }

 private:
  std::filesystem::path path_;
};

// --- Outside -----------------------------------------------------------------------

TEST(Story1_EightMembersAtStart) {
  State s = New();
  REQUIRE(TheSet.size() == 8, "the set does not have 8 members");
  REQUIRE(s.Alive.size() == 8, "campaign does not start with 8 alive");
  std::vector<MemberID> seen;
  for (const auto& m : TheSet) {
    REQUIRE(!Contains(seen, m.ID), "duplicate member");
    seen.push_back(m.ID);
  }
}

TEST(Story2_EveningsAreFiniteAndTheOnlyWayToKnowSomeone) {
  State s = New();

  int spent = 0;
  while (!s.Outside.empty()) {
    s = Must(Apply(s, SpendEvening{s.Outside[0]}));
    spent++;
    REQUIRE(spent <= 8, "a chapter never ran out of evenings");
  }
  REQUIRE(spent > 0, "a chapter gave no evenings at all");

  int before = s.Evenings(Marcus);
  BipOut bip;
  bip.Seat = Seat::OnBack;
  bip.Companion = Marcus;
  bip.CompanionReturned = true;
  s = Must(Apply(s, bip));
  CHECK(s.Evenings(Marcus) == before, "a bip changed familiarity with Marcus");
}

TEST(Story3_EveningsReachOneManOnly) {
  State s = New();
  std::vector<int> before;
  for (const auto& m : TheSet) before.push_back(s.Evenings(m.ID));

  s = Must(Apply(s, SpendEvening{Marcus}));

  for (size_t i = 0; i < TheSet.size(); ++i) {
    int want = before[i] + (TheSet[i].ID == Marcus ? 1 : 0);
    CHECK(s.Evenings(TheSet[i].ID) == want, "evenings moved for the wrong member");
  }
}

TEST(Story4_CannotKnowAllEightWell) {
  int maxPerChapter =
      (static_cast<int>(TheSet.size()) + DepartAlongside) / (1 + DepartAlongside);
  REQUIRE(maxPerChapter * Chapters < static_cast<int>(TheSet.size()) * EveningsToClose,
          "a campaign yields enough evenings to know all eight well");

  State s = New();
  while (s.Running()) {
    while (!s.Outside.empty()) {
      MemberID least = s.Outside[0];
      for (const auto& id : s.Outside) {
        if (s.Evenings(id) < s.Evenings(least)) least = id;
      }
      s = Must(Apply(s, SpendEvening{least}));
    }
    BipOut bip;
    bip.Seat = Seat::Riding;
    bip.Companion = s.Alive[0];
    bip.CompanionReturned = true;
    s = Must(Apply(s, bip));
  }
  int close = 0;
  for (const auto& m : TheSet) {
    if (s.Knows(m.ID) == Level::Close) close++;
  }
  REQUIRE(close != static_cast<int>(TheSet.size()),
          "a perfect campaign came to know all eight well");
}

TEST(Story5_FamiliarityIsNeverLost) {
  State s = New();
  s = Must(Apply(s, SpendEvening{Marcus}));
  // Tyrone is one of the few still out after an evening with Marcus — the others
  // went in while Jamal was under a car.
  s = Must(Apply(s, SpendEvening{Tyrone}));
  int marcus = s.Evenings(Marcus), tyrone = s.Evenings(Tyrone);

  BipOut bip;
  bip.Seat = Seat::Riding;
  bip.Companion = Tyrone;
  bip.CompanionReturned = false;
  s = Must(Apply(s, bip));
  REQUIRE(!s.IsAlive(Tyrone), "Tyrone survived a bip he did not return from");
  CHECK(s.Evenings(Marcus) == marcus, "Marcus lost familiarity when Tyrone died");
  CHECK(s.Evenings(Tyrone) == tyrone, "Tyrone's familiarity was reset by his death");
}

// --- Preparing the ride ------------------------------------------------------------

TEST(Story8_SeatIsChosenEveryBip) {
  State s = New();
  BipOut noSeat;
  noSeat.Companion = Marcus;
  noSeat.CompanionReturned = true;
  REQUIRE(Apply(s, noSeat).Err == Error::Seat, "a bip with no seat was allowed");

  for (Seat seat : {Seat::Riding, Seat::OnBack}) {
    BipOut bip;
    bip.Seat = seat;
    bip.Companion = Marcus;
    bip.CompanionReturned = true;
    CHECK(Apply(s, bip).Ok(), "a valid seat was rejected");
  }
}

TEST(Story9_SeatIsNeverLockedInByTheLastOne) {
  State s = New();
  BipOut first;
  first.Seat = Seat::Riding;
  first.Companion = Marcus;
  first.CompanionReturned = true;
  s = Must(Apply(s, first));

  BipOut opposite;
  opposite.Seat = Seat::OnBack;
  opposite.Companion = Marcus;
  opposite.CompanionReturned = true;
  CHECK(Apply(s, opposite).Ok(), "could not switch seat after riding");

  BipOut same;
  same.Seat = Seat::Riding;
  same.Companion = Marcus;
  same.CompanionReturned = true;
  CHECK(Apply(s, same).Ok(), "could not keep the same seat");
}

TEST(Story10_ExactlyOneMemberFillsTheOtherSeat) {
  State s = New();
  BipOut noCompanion;
  noCompanion.Seat = Seat::Riding;
  noCompanion.CompanionReturned = true;
  REQUIRE(Apply(s, noCompanion).Err == Error::NotAMember,
          "a bip with no companion was allowed");

  BipOut invented;
  invented.Seat = Seat::Riding;
  invented.Companion = "nobody";
  invented.CompanionReturned = true;
  REQUIRE(Apply(s, invented).Err == Error::NotAMember,
          "a bip with an invented companion was allowed");
}

TEST(Story11_RideIsCappedByTheBikeNotByARule) {
  // Enforced at compile time in campaign.hpp; see the static_asserts there. This test
  // exists so "one test per story" still names story 11 somewhere runnable.
  static_assert(std::is_same_v<decltype(BipOut::Companion), MemberID>,
                "BipOut.Companion must be a single MemberID");
}

TEST(Story12_OnlyTheLivingCanBeBrought) {
  State s = New();
  BipOut lost;
  lost.Seat = Seat::Riding;
  lost.Companion = Omar;
  lost.CompanionReturned = false;
  s = Must(Apply(s, lost));

  BipOut bringDead;
  bringDead.Seat = Seat::Riding;
  bringDead.Companion = Omar;
  bringDead.CompanionReturned = true;
  REQUIRE(Apply(s, bringDead).Err == Error::Dead, "a dead man was brought on a bip");

  for (const auto& id : s.Companions()) {
    CHECK(s.IsAlive(id), "a dead member is offered as a companion");
  }
}

TEST(Story13_NothingCanGrowTheSet) {
  State s = New();
  size_t start = s.Alive.size();
  while (s.Running()) {
    s = SpendAll(s, s.Cycle % 4 != 0);  // lose someone every fourth bip
    REQUIRE(s.Alive.size() <= start, "the roster grew");
  }
  for (const auto& id : s.Alive) {
    Member m;
    CHECK(MemberByID(id, m), "a living member is not one of the eight");
  }
}

TEST(Story14_MemberCarriesNoNumber) {
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

  Member m;
  REQUIRE(MemberByID(Daquavion, m), "Daquavion not found");
  CHECK(!m.CostOfLoss.empty() && !m.Cap.Riding.empty() && !m.Cap.OnBack.empty(),
        "a member does not expose who he is in both seats and what his loss costs");
}

// --- Bipping out ---------------------------------------------------------------

TEST(Story15_EncounterOutcomeEntersAsAResult) {
  State base = New();
  BipOut win;
  win.Seat = Seat::Riding;
  win.Companion = Marcus;
  win.CompanionReturned = true;
  State lived = Must(Apply(base, win));

  BipOut loss;
  loss.Seat = Seat::Riding;
  loss.Companion = Marcus;
  loss.CompanionReturned = false;
  State died = Must(Apply(base, loss));

  CHECK(lived.IsAlive(Marcus), "Marcus returned but was recorded dead");
  CHECK(!died.IsAlive(Marcus), "Marcus did not return but was recorded alive");
}

TEST(Story16_EveryBipEscalatesWhateverHappened) {
  for (bool returned : {true, false}) {
    State s = New();
    int before = s.Cycle;
    BipOut bip;
    bip.Seat = Seat::OnBack;
    bip.Companion = James;
    bip.CompanionReturned = returned;
    s = Must(Apply(s, bip));
    CHECK(s.Cycle > before, "cycle did not advance");
  }
}

TEST(Story17_WinningCleanlyStillEscalates) {
  State s = New();
  for (int i = 0; i < 5; ++i) {
    int before = s.Cycle;
    BipOut bip;
    bip.Seat = Seat::Riding;
    bip.Companion = Anthony;
    bip.CompanionReturned = true;
    s = Must(Apply(s, bip));
    REQUIRE(s.Cycle == before + 1, "clean bip did not escalate");
  }
  REQUIRE(s.Alive.size() == 8, "nobody should have died");
  REQUIRE(s.Cycle == 5, "cycle is wrong after five clean bips");
}

TEST(Story18_EscalationIsMonotonic) {
  State s = New();
  int last = s.Cycle, lastIntensity = s.Intensity();
  while (s.Running()) {
    s = SpendAll(s, s.Cycle % 3 != 0);
    REQUIRE(s.Cycle >= last, "cycle went backwards");
    REQUIRE(s.Intensity() >= lastIntensity, "intensity went backwards");
    last = s.Cycle;
    lastIntensity = s.Intensity();
  }
}

TEST(IntensityPlateaus) {
  // ADR 0013: escalation climbs early and then stops.
  State s = New();
  while (s.Running()) s = SpendAll(s, true);
  REQUIRE(s.Intensity() == IntensityCeiling, "intensity is not at the ceiling");
  REQUIRE(s.Cycle > IntensityCeiling, "the cycle should have run past the ceiling");
}

// --- Loss and persistence -------------------------------------------------------

TEST(Story22_DeathIsPermanent) {
  State s = New();
  BipOut bip;
  bip.Seat = Seat::Riding;
  bip.Companion = Tyrone;
  bip.CompanionReturned = false;
  s = Must(Apply(s, bip));
  REQUIRE(!s.IsAlive(Tyrone), "Tyrone is alive after not returning");
  while (s.Running()) {
    s = SpendAll(s, true);
    REQUIRE(!s.IsAlive(Tyrone), "Tyrone came back");
  }
}

TEST(Story23_RosterOnlyShortens) {
  State s = New();
  size_t last = s.Alive.size();
  while (s.Running()) {
    s = SpendAll(s, s.Cycle % 2 == 0);
    REQUIRE(s.Alive.size() <= last, "roster grew");
    last = s.Alive.size();
  }
}

TEST(Story24_DeathIsNeverAFailState) {
  State s = New();
  BipOut bip;
  bip.Seat = Seat::Riding;
  bip.Companion = Joshua;
  bip.CompanionReturned = false;
  Result r = Apply(s, bip);
  REQUIRE(r.Ok(), "losing a man returned an error");
  REQUIRE(r.S.Running(), "losing a man ended the campaign");

  s = r.S;
  while (s.Running()) {
    BipOut b;
    b.Seat = Seat::Riding;
    b.Companion = s.Alive[0];
    b.CompanionReturned = false;
    s = Must(Apply(s, b));
  }
  REQUIRE(s.End == EndReason::SetEmpty, "campaign did not end as set-empty");
  REQUIRE(s.Alive.empty(), "the set should be empty");
}

TEST(Story25_PlayableWithAnySubsetAlive) {
  for (const auto& first : TheSet) {
    State s = New();
    BipOut bip;
    bip.Seat = Seat::OnBack;
    bip.Companion = first.ID;
    bip.CompanionReturned = false;
    s = Must(Apply(s, bip));
    int steps = 0;
    while (s.Running()) {
      s = SpendAll(s, s.Alive.size() > 1);
      steps++;
      REQUIRE(steps <= Chapters + 2,
              "losing a member first produced a campaign that would not end");
    }
    REQUIRE(s.End != EndReason::NotEnded, "losing a member first left it hanging");
  }
}

TEST(Story26_StateRoundTrips) {
  State s = New();
  s = Must(Apply(s, SpendEvening{Marcus}));
  BipOut bip;
  bip.Seat = Seat::OnBack;
  bip.Companion = Alex;
  bip.CompanionReturned = false;
  s = Must(Apply(s, bip));

  TempDir dir;
  REQUIRE(Save(dir.String(), s), "save failed");
  State back;
  REQUIRE(Load(dir.String(), back), "load failed");
  REQUIRE(StatesEqual(s, back), "state did not survive the round trip");

  // And it resumes: the next decision behaves identically either way.
  State a = Must(Apply(s, SpendEvening{Omar}));
  State b = Must(Apply(back, SpendEvening{Omar}));
  REQUIRE(StatesEqual(a, b), "a restored campaign did not resume where it stopped");
}

TEST(Story27_OneSlotThatAdvancesOnItsOwn) {
  TempDir dir;
  State s = New();
  for (int i = 0; i < 4; ++i) {
    if (!s.Outside.empty()) {
      s = Must(Apply(s, SpendEvening{s.Outside[0]}));
    } else {
      BipOut bip;
      bip.Seat = Seat::Riding;
      bip.Companion = s.Alive[0];
      bip.CompanionReturned = true;
      s = Must(Apply(s, bip));
    }
    REQUIRE(Save(dir.String(), s), "save failed");
  }
  int count = 0;
  std::string sawName;
  for (const auto& entry : std::filesystem::directory_iterator(dir.String())) {
    count++;
    sawName = entry.path().filename().string();
  }
  REQUIRE(count == 1 && sawName == SlotName,
          "saving four times did not leave exactly one slot");

  // No API exists for naming a slot or restoring an earlier one: Save and Load take a
  // directory and nothing else, enforced by these signatures compiling at all.
  static_assert(std::is_same_v<decltype(&Save), bool (*)(const std::string&, const State&)>);
  static_assert(std::is_same_v<decltype(&Load), bool (*)(const std::string&, State&)>);
}

TEST(Story28_IdenticalDecisionsReproduceIdenticalState) {
  std::vector<Decision> decisions = {
      SpendEvening{Marcus},
      SpendEvening{Tyrone},
      BipOut{Seat::Riding, Daquavion, false},
      SpendEvening{Omar},
      BipOut{Seat::OnBack, Tyrone, true},
      SpendEvening{Anthony},
      BipOut{Seat::Riding, James, false},
  };
  auto run = [&]() {
    State s = New();
    for (const auto& d : decisions) s = Must(Apply(s, d));
    return s;
  };

  State first = run(), second = run();
  REQUIRE(StatesEqual(first, second), "the same decisions produced different states");

  // And across a save: stop halfway, reload, finish. Same answer.
  TempDir dir;
  State s = New();
  for (size_t i = 0; i < 3; ++i) s = Must(Apply(s, decisions[i]));
  REQUIRE(Save(dir.String(), s), "save failed");
  State resumed;
  REQUIRE(Load(dir.String(), resumed), "load failed");
  for (size_t i = 3; i < decisions.size(); ++i) resumed = Must(Apply(resumed, decisions[i]));
  REQUIRE(StatesEqual(resumed, first), "resuming from a save diverged");
}

// --- Done looks like -------------------------------------------------------------

TEST(FullCampaignRunsWithNoEngine) {
  State s = New();
  int chapters = 0;
  while (s.Running()) {
    // Lose someone roughly one bip in four, which is the tuning target.
    s = SpendAll(s, (s.Cycle + 1) % 4 != 0);
    chapters++;
  }
  REQUIRE(chapters == Chapters, "campaign ran the wrong number of chapters");
  REQUIRE(s.End == EndReason::Finished, "campaign did not end as finished");
  REQUIRE(!s.Alive.empty() && s.Alive.size() != 8,
          "at one bip in four the set should be diminished, not intact or empty");
  std::fprintf(stderr, "    finished chapter %d with %zu of 8 alive, cycle %d, intensity %d\n",
               Chapters, s.Alive.size(), s.Cycle, s.Intensity());
}

}  // namespace
}  // namespace campaign::test

int main() {
  int passed = 0, failed = 0;
  for (const auto& tc : campaign::test::Registry()) {
    try {
      tc.fn();
      std::printf("PASS %s\n", tc.name.c_str());
      passed++;
    } catch (const campaign::test::Failure& f) {
      std::printf("FAIL %s: %s\n", tc.name.c_str(), f.msg.c_str());
      failed++;
    } catch (const std::exception& e) {
      std::printf("FAIL %s: unexpected exception: %s\n", tc.name.c_str(), e.what());
      failed++;
    }
  }
  std::printf("\n%d passed, %d failed, %d total\n", passed, failed,
              passed + failed);
  return failed == 0 ? 0 : 1;
}
