// Port of campaign/apply.go. Apply is the seam and the whole reason this module
// exists in either language.
#include "campaign.hpp"

namespace campaign {

// Declared and defined in state.cpp; not part of the public header because Go's
// equivalents (contains, remove, inSetOrder) are unexported too.
std::vector<MemberID> Remove(const std::vector<MemberID>& ids, const MemberID& id);
std::vector<MemberID> InSetOrder(const std::vector<MemberID>& ids);
bool Contains(const std::vector<MemberID>& ids, const MemberID& id);

namespace {

Result applySpendEvening(const State& s, const SpendEvening& d) {
  Member m;
  if (!MemberByID(d.With, m)) return Result{s, Error::NotAMember};
  if (!s.IsAlive(d.With)) return Result{s, Error::Dead};
  if (!s.IsOutside(d.With)) return Result{s, Error::GoneInside};

  State out = s.Clone();

  // Only this member's familiarity moves. Never the set's (story 3).
  out.SetEvenings(d.With, out.Evenings(d.With) + 1);

  // He goes in, and so do some others — you were with him, so you were not there
  // when they left (ADR 0012). Deterministic, and never announced.
  out.Outside = Remove(out.Outside, d.With);

  // depart sends DepartAlongside members inside, choosing them deterministically: the
  // ones standing after the member Jamal chose, in the set's order, wrapping around.
  //
  // Deterministic is required — story 28 needs identical inputs to reproduce identical
  // state, and randomising this would make permanence an artefact of the save system
  // rather than a rule. Deterministic is not the same as legible: a first-time player
  // has no way to know the pattern, and autosave-only denies them the repetition that
  // would teach it (ADR 0012).
  if (!out.Outside.empty()) {
    std::vector<MemberID> ordered = InSetOrder(out.Outside);

    int start = 0;
    for (size_t i = 0; i < TheSet.size(); ++i) {
      if (TheSet[i].ID == d.With) {
        start = static_cast<int>(i);
        break;
      }
    }

    std::vector<MemberID> leaving;
    for (int i = 1; i <= static_cast<int>(TheSet.size()) &&
                    static_cast<int>(leaving.size()) < DepartAlongside;
         ++i) {
      const MemberID& candidate = TheSet[(start + i) % TheSet.size()].ID;
      if (Contains(ordered, candidate) && !Contains(leaving, candidate)) {
        leaving.push_back(candidate);
      }
    }

    std::vector<MemberID> staying;
    staying.reserve(ordered.size());
    for (const auto& mem : ordered) {
      if (!Contains(leaving, mem)) staying.push_back(mem);
    }
    out.Outside = staying;
  }

  return Result{out, Error::None};
}

Result applyBipOut(const State& s, const BipOut& d) {
  if (d.Seat != Seat::Riding && d.Seat != Seat::OnBack) return Result{s, Error::Seat};
  Member m;
  if (!MemberByID(d.Companion, m)) return Result{s, Error::NotAMember};
  // Only the living can be brought (story 12). Note there is no check that he is
  // still outside: who fills the seat is not limited by who you happened to catch
  // this evening.
  if (!s.IsAlive(d.Companion)) return Result{s, Error::Dead};

  State out = s.Clone();

  // The cycle advances on completion, whatever happened (stories 16, 17), and
  // nothing anywhere reduces it (story 18). A bip won cleanly, losing nobody, still
  // escalates.
  out.Cycle++;

  // The result the encounter handed us becomes permanent. No revival, no
  // replacement, and no path back onto the roster (stories 22, 23).
  if (!d.CompanionReturned) {
    out.Alive = Remove(out.Alive, d.Companion);
  }

  // What happened here is what the Block answers next chapter (ADR 0011). Recorded
  // rather than derived: once the roster has shortened there is no way to read back
  // out of it whether tonight cost someone or whether everyone came home.
  out.LastBip = LastBipInfo{true, !d.CompanionReturned, d.Companion};

  out.Chapter++;

  if (out.Alive.empty()) {
    // The loop cannot run with nobody to fill the other seat, so the campaign
    // reaches its ending early. This is arrival, not failure (ADR 0007).
    out.End = EndReason::SetEmpty;
    out.Outside.clear();
  } else if (out.Chapter > Chapters) {
    out.End = EndReason::Finished;
    out.Outside.clear();
  } else {
    // A new evening. Everyone living is outside when a chapter opens.
    out.Outside = out.Alive;
  }

  return Result{out, Error::None};
}

}  // namespace

Result Apply(const State& s, const Decision& d) {
  if (!s.Running()) return Result{s, Error::Ended};
  if (auto* se = std::get_if<SpendEvening>(&d)) return applySpendEvening(s, *se);
  if (auto* bo = std::get_if<BipOut>(&d)) return applyBipOut(s, *bo);
  return Result{s, Error::NotAMember};  // unreachable: Decision is a closed variant
}

}  // namespace campaign
