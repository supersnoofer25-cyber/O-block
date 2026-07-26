// Port of campaign/outside.go. Ambient content for time outside: where each member is
// tonight, and why he goes in when he goes. See outside.go for the full reasoning
// behind why this exists per chapter and not once (ADR 0012) — not repeated here.
//
// Lookups are by key, so the container is not an ordering and story 28 is unaffected.
#include "campaign.hpp"

#include <map>

namespace campaign {

namespace {

const std::map<MemberID, std::vector<std::string>> tonight = {
    {Alex,
     {
         "on the steps, his sister doing homework beside him",
         "walking his sister back from the shop",
         "on the wall, his sister and two of her friends round him",
         "outside his building while his sister skips rope",
         "on the kerb, his sister arguing with someone about nothing",
     }},
    {Tyrone,
     {
         "at his spot, working a queue that isn't moving",
         "on the corner arguing prices with someone who isn't buying",
         "at his spot with nothing shifting, talking anyway",
         "leaning on the shutter, holding court",
         "at his spot, packing up slow",
     }},
    {Anthony,
     {
         "in his work clothes, not changed yet",
         "before his shift, killing twenty minutes",
         "just off, still smelling of the job",
         "on the wall with his bag between his feet",
         "waiting on a lift to work that hasn't come",
     }},
    {James,
     {
         "up on the church roof with a bucket",
         "on the roof, sat down, not working",
         "round the back of the church with the ladder",
         "up top, looking at the same patch as last week",
         "on the church steps with dust all over him",
     }},
    {Daquavion,
     {
         "on his step with the baby on his shoulder",
         "outside his place, rocking the pram with his foot",
         "in the doorway while the baby sleeps inside",
         "walking the baby up and down to settle her",
         "on the wall, checking his phone every minute",
     }},
    {Marcus,
     {
         "under a car with only his legs showing",
         "bent into somebody's engine",
         "under the same car he was under last time",
         "jump-starting something for whoever asked",
         "wiping his hands on a rag, done for now",
     }},
    {Omar,
     {
         "at the hoop, taking the same shot over",
         "at the hoop with nobody to play",
         "sat on the ball, not playing",
         "playing whoever turned up",
         "at the hoop long after everyone else went in",
     }},
    {Joshua,
     {
         "on your step, because he lives there",
         "in the doorway of your house",
         "on the wall outside yours",
         "at home, waiting for you to get back",
         "on the step with the door open behind him",
     }},
};

const std::map<MemberID, std::vector<std::string>> wentIn = {
    {Alex, {"takes his sister up", "gets called in by his mum", "walks his sister home and doesn't come back"}},
    {Tyrone, {"counts up and goes", "packs it in, nothing moving", "is done for the night"}},
    {Anthony, {"has work", "goes up to sleep before his shift", "gets his lift at last"}},
    {James, {"climbs down when the light goes", "can't see what he's doing any more", "puts the ladder round the back"}},
    {Daquavion, {"takes the baby in", "gets called up by his girl", "goes in when she finally goes down"}},
    {Marcus, {"goes to wash his hands", "runs out of light", "goes in to eat"}},
    {Omar, {"gets called in", "has had enough", "goes in when it gets cold"}},
    {Joshua, {"goes up — it's his house too", "is inside before you are", "shuts the door behind him"}},
};

// rotate picks a line deterministically, which story 28 requires — identical decision
// sequences must reproduce identical play, and a random pick here would make what the
// player saw depend on when they saw it.
//
// Each man is offset by his place in the set so the eight do not advance in lockstep.
// Lockstep is what would turn this into the schedule ADR 0012 forecloses: the whole
// Block changing on the same beat is a pattern to read, where eight cycles of
// different lengths sliding past each other is just the Block.
int rotate(const MemberID& id, int chapter, int n) {
  int offset = 0;
  for (size_t i = 0; i < TheSet.size(); ++i) {
    if (TheSet[i].ID == id) {
      offset = static_cast<int>(i);
      break;
    }
  }
  if (chapter < 1) chapter = 1;
  return (chapter - 1 + offset) % n;
}

}  // namespace

std::string Tonight(const MemberID& id, int chapter) {
  auto it = tonight.find(id);
  if (it == tonight.end() || it->second.empty()) {
    Member m;
    if (MemberByID(id, m)) return m.Activity;
    return "";
  }
  return it->second[rotate(id, chapter, static_cast<int>(it->second.size()))];
}

std::string WentIn(const MemberID& id, int chapter) {
  auto it = wentIn.find(id);
  if (it == wentIn.end() || it->second.empty()) return "went in";
  return it->second[rotate(id, chapter, static_cast<int>(it->second.size()))];
}

}  // namespace campaign
