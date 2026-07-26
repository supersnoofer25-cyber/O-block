// Port of campaign/flare.go. See flare.go for the two ADR 0011 rules held structurally
// rather than editorially — a flare never kills a set member and never touches a cost
// of loss — which are unchanged by the port and not repeated here.
#include "campaign.hpp"

#include <cstdio>

namespace campaign {

namespace {

// The beef is inherited, so chapter one needs no special case — its reason is simply
// the most recent thing that happened before the game began. It is also the one flare
// that cannot say why any of it started, because nothing can (ADR 0010).
const std::string inherited =
    "Nobody has to tell you why. Thursday they came through twice, slow, with the "
    "windows down, and everyone out saw it. It was going before you were old enough to "
    "be in it, and no two people tell the same story about how.";

// They answer a bip that cost the player nothing. He got out clean; they did not
// agree to let it go.
const std::vector<std::vector<std::string>> answered = {
    {
        "Somebody has it before you're properly out, then somebody else tells it "
        "different. They came back through in the night and did the windows along the "
        "low block. Nobody was in. Everyone knows what it was for.",
        "It goes round before you're down the steps. A car came through twice in the "
        "night, slow, and went again without stopping. Nothing happened. It didn't "
        "have to.",
    },
    {
        "You get told three times before you reach the corner and it's worse each "
        "time. They came through with the lights off and put the door of the low "
        "block in.",
        "Everyone has a different version by the time it reaches you. Theirs came "
        "through before it was light and did the stairwell — every window on the way "
        "up.",
        "Somebody's brother saw it and can't stop telling it. They came through, took "
        "the corner too fast, and did it anyway on the way back round.",
    },
    {
        "Nobody bothers telling it properly any more. They came through in the night. "
        "They'll come through again, and after that they'll come through again.",
        "You hear it the way you hear the weather. They came through. There's a car "
        "on its roof at the end of the road and somebody will move it eventually.",
        "It reaches you late and nobody tells it like news. They came through again. "
        "The people who used to come out to look don't come out any more.",
        "Nobody says much about it. They came through in the night, and everyone knew "
        "they would, which turns out not to be the same as being ready for it.",
        "Somebody mentions it in the middle of something else. They came through. It "
        "doesn't interrupt what he was saying.",
    },
};

// The player lost somebody last time, so the Block goes for him. One %s, his name.
//
// This is the flare that most needs to read as aggression rather than as consequence
// (ADR 0011): the causal chain is completely visible, nothing is concealed, and from
// inside it still feels like the only thing to do.
const std::vector<std::vector<std::string>> goingFor = {
    {
        "Nobody organises it. By the time you're outside it's already settled where "
        "everyone is going tonight, and it's for %s.",
        "You don't get asked. Everyone out has decided what tonight is before you've "
        "said anything, and it's for %s.",
    },
    {
        "It's decided before you're down the steps. Nobody says %s's name, and it's "
        "the only thing anybody is talking about.",
        "Nobody puts it into words. It's for %s, and there is nothing else it could "
        "be for.",
        "Somebody starts to say %s's name and stops. Everyone out knows what tonight "
        "is anyway.",
    },
    {
        "Nobody discusses it any more. It's for %s, the way the last one was for "
        "somebody, the way the next one will be.",
        "It's for %s. Nobody says so and nobody needs to, and by now that's just how "
        "a night starts.",
        "You get told about %s the way you'd get told the time. It's for him. It was "
        "always going to be.",
        "Nobody makes anything of it. It's for %s, and everyone still out has done "
        "this enough to know their part without being told.",
        "It's for %s. You notice that nobody argues, and that you didn't expect "
        "anybody to.",
    },
};

// People on the Block who are not in the set. Named, placed and recognisable —
// someone the player could have seen outside — because ADR 0011 rejects anonymous
// casualties. None of these places or people is part of anybody's cost of loss.
const std::vector<std::string> bystanders = {
    "Renée, who works the launderette counter, was on the pavement when it happened. "
    "She had nothing to do with any of it.",
    "Curtis got hit at the bus stop. He was waiting for a bus.",
    "Miss Pat, top floor of the low block, was at her window. Nobody on the Block says "
    "very much about it.",
    "Kenny was locking up the phone shop. He'd been on the Block longer than any of "
    "you and was not part of anything.",
};

// band collapses intensity into the three registers the flares are written in. The
// top one is where the campaign spends most of its length, by design.
int band(int intensity) {
  if (intensity <= 1) return 0;
  if (intensity <= 3) return 1;
  return 2;
}

// killed decides whether tonight's flare took somebody, deterministically and not
// every time.
bool killed(const State& s) { return s.LastBip.Happened && s.Cycle % 3 == 2; }

std::string formatName(const std::string& line, const std::string& name) {
  // A minimal stand-in for fmt.Sprintf(line, name): every flare line has exactly one
  // "%s", replaced with the dead or targeted member's name. No general format spec
  // support is needed because nothing here ever writes a second one.
  std::string out;
  out.reserve(line.size() + name.size());
  size_t pos = line.find("%s");
  out.append(line, 0, pos);
  out.append(name);
  out.append(line, pos + 2, std::string::npos);
  return out;
}

// news picks the line. Kind comes from the last bip; severity comes from intensity,
// which climbs over the first third of the campaign and then sits at its ceiling
// (ADR 0013).
std::string news(const State& s) {
  if (!s.LastBip.Happened) return inherited;

  const std::vector<std::vector<std::string>>* lines = &answered;
  if (s.LastBip.Lost) lines = &goingFor;

  // Within a band, rotate on the cycle so consecutive chapters do not open on the
  // same sentence.
  const std::vector<std::string>& at = (*lines)[band(s.Intensity())];
  const std::string& line = at[s.Cycle % at.size()];

  if (s.LastBip.Lost) {
    Member m;
    MemberByID(s.LastBip.Who, m);
    return formatName(line, m.Name);
  }
  return line;
}

}  // namespace

Flare FlareFor(const State& s) {
  Flare f;
  f.News = news(s);
  if (killed(s)) {
    // Indexed by how many flares have taken somebody, not by the cycle. Cycle is
    // what decided that this is one of those chapters, so using it again here picked
    // the same bystander every time.
    f.Dead = bystanders[(s.Cycle / 3) % bystanders.size()];
  }
  return f;
}

}  // namespace campaign
