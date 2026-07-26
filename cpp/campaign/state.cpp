// Port of campaign/state.go: New() and every State method that isn't Apply itself.
// See campaign.hpp for the type-level translation notes.
#include "campaign.hpp"

#include <algorithm>

namespace campaign {

bool Contains(const std::vector<MemberID>& ids, const MemberID& id) {
  return std::find(ids.begin(), ids.end(), id) != ids.end();
}

// Remove returns ids without id, preserving order.
std::vector<MemberID> Remove(const std::vector<MemberID>& ids, const MemberID& id) {
  std::vector<MemberID> out;
  out.reserve(ids.size());
  for (const auto& x : ids) {
    if (x != id) out.push_back(x);
  }
  return out;
}

// InSetOrder sorts ids into TheSet's declared order, so that no operation can leave a
// collection in an order that depends on how it was built (story 28).
std::vector<MemberID> InSetOrder(const std::vector<MemberID>& ids) {
  // Rank is found by scanning TheSet rather than building a lookup map keyed on
  // MemberID — the set is eight members, fixed, so a map would cost more to build
  // than the scan it replaces.
  auto rankOf = [](const MemberID& id) -> int {
    for (size_t i = 0; i < TheSet.size(); ++i) {
      if (TheSet[i].ID == id) return static_cast<int>(i);
    }
    return static_cast<int>(TheSet.size());
  };
  std::vector<MemberID> out = ids;
  std::stable_sort(out.begin(), out.end(), [&](const MemberID& a, const MemberID& b) {
    return rankOf(a) < rankOf(b);
  });
  return out;
}

State New() {
  State s;
  s.Chapter = 1;
  for (const auto& m : TheSet) {
    s.Alive.push_back(m.ID);
    s.Familiarity.push_back(FamiliarityEntry{m.ID, 0});
  }
  s.SetEvenings(Joshua, EveningsToClose);
  s.Outside = s.Alive;
  return s;
}

bool State::IsAlive(const MemberID& id) const { return Contains(Alive, id); }

bool State::IsOutside(const MemberID& id) const { return Contains(Outside, id); }

int State::Evenings(const MemberID& id) const {
  for (const auto& f : Familiarity) {
    if (f.Member == id) return f.Evenings;
  }
  return 0;
}

Level State::Knows(const MemberID& id) const {
  int n = Evenings(id);
  if (n >= EveningsToClose) return Level::Close;
  if (n >= EveningsToClose / 2) return Level::Known;
  if (n > 0) return Level::Acquainted;
  return Level::Stranger;
}

int State::Intensity() const {
  if (Cycle > IntensityCeiling) return IntensityCeiling;
  return Cycle;
}

std::string State::Reading(const MemberID& id, Seat jamalSeat) const {
  Member m;
  if (!MemberByID(id, m)) return "";

  // His seat is the other one. Every capability is defined in both, so which half the
  // player sees follows from where Jamal sits and not from who the man is (ADR 0004,
  // ADR 0009).
  std::string his = m.Cap.OnBack;
  if (jamalSeat == Seat::OnBack) his = m.Cap.Riding;

  switch (Knows(id)) {
    case Level::Stranger:
      // Not a worse man. A man Jamal cannot read.
      return m.Cap.Name + ". " + m.Cap.Unread + ".";
    case Level::Close:
      return m.Cap.Name + ". " + his + ". " + m.Cap.Unprompted + ".";
    default:
      return m.Cap.Name + ". " + his + ".";
  }
}

void State::SetEvenings(const MemberID& id, int n) {
  for (auto& f : Familiarity) {
    if (f.Member == id && n > f.Evenings) {
      f.Evenings = n;
      return;
    }
  }
}

}  // namespace campaign
