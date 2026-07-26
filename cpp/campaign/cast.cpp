// Port of campaign/cast.go. The eight. Names and content are docs/cast.md; this is
// that file expressed as data, same as the Go source it's translated from — content is
// transcribed verbatim, not reworded, so this file and cast.go should never disagree.
#include "campaign.hpp"

namespace campaign {

const std::vector<Member> TheSet = {
    Member{
        .ID = Alex,
        .Name = "Alex",
        .Cap =
            Capability{
                .Name = "Doesn't hesitate",
                .Riding = "Commits to the approach, closer than you'd have chosen",
                .OnBack = "Fires first, before you've decided this is happening",
                .Unread =
                    "You haven't been round him enough to tell the difference between "
                    "him committing and him going further than you'd have chosen",
                .Unprompted =
                    "He says what he's going to do before he does it, so you're never "
                    "guessing which it is",
            },
        .CostOfLoss =
            "His little sister stops coming outside. She is still on the Block; you "
            "just don't see her any more.",
        .Activity = "around, with his sister there",
        .Tie = Daquavion,
    },
    Member{
        .ID = Tyrone,
        .Name = "Tyrone",
        .Cap =
            Capability{
                .Name = "Won't shut up",
                .Riding = "Calling what's ahead before you reach it",
                .OnBack = "Telling you what's behind you",
                .Unread =
                    "It's all noise to you, and you can't yet tell which of it you're "
                    "meant to act on",
                .Unprompted =
                    "You know by now which of it is nerves and which of it is him "
                    "telling you something",
            },
        .CostOfLoss = "He sells food. After Tyrone the Block doesn't eat the way it did.",
        .Activity = "at his spot while he sells",
        .Tie = James,
    },
    Member{
        .ID = Anthony,
        .Name = "Anthony",
        .Cap =
            Capability{
                .Name = "The one who says no",
                .Riding = "Takes the careful exit",
                .OnBack = "Holds until he's sure",
                .Unread =
                    "You don't know what makes him say it, so when he does you've only "
                    "got his word for it",
                .Unprompted =
                    "When he says no he tells you what he saw, and you can decide with "
                    "him instead of after him",
            },
        .CostOfLoss =
            "He has a job. After Anthony nobody on the Block is working — the thread "
            "to an ordinary life goes with him.",
        .Activity = "before or after work",
        .Tie = Marcus,
    },
    Member{
        .ID = James,
        .Name = "James",
        .Cap =
            Capability{
                .Name = "Doesn't leave early",
                .Riding = "Won't pull out while anyone is still exposed",
                .OnBack = "Keeps firing until it's actually done",
                .Unread = "You can't tell how long he means to stay, so you can't plan "
                          "around him",
                .Unprompted =
                    "He tells you what he's waiting on, so it isn't a surprise when he "
                    "won't move",
            },
        .CostOfLoss = "He fixes the church roof. After James it doesn't get fixed, and "
                      "the Block uses less of where it gathers.",
        .Activity = "up on the church roof",
        .Tie = Tyrone,
    },
    Member{
        .ID = Daquavion,
        .Name = "Daquavion",
        .Cap =
            Capability{
                .Name = "Moves first",
                .Riding = "Already moving when it goes wrong",
                .OnBack = "Already firing when it goes wrong",
                .Unread = "He's gone before you've registered why, and you spend the "
                          "whole thing behind him",
                .Unprompted =
                    "You've been out with him enough to move when he moves, which is "
                    "most of it",
            },
        .CostOfLoss = "He has a newborn. After Daquavion his girl takes the baby off "
                      "the Block.",
        .Activity = "at his place, around the baby",
        .Tie = Alex,
    },
    Member{
        .ID = Marcus,
        .Name = "Marcus",
        .Cap =
            Capability{
                .Name = "Knows the way out",
                .Riding = "Takes exits you wouldn't have found",
                .OnBack = "Telling you where to go before you've asked",
                .Unread = "Where he's taking you is his business, because you've never "
                          "once asked",
                .Unprompted = "He talks the route through before you go, and by now "
                              "you know it as well as he does",
            },
        .CostOfLoss = "He works on cars. After Marcus they stop running — not his, "
                      "everyone's.",
        .Activity = "under a car",
        .Tie = Anthony,
    },
    Member{
        .ID = Omar,
        .Name = "Omar",
        .Cap =
            Capability{
                .Name = "Acts like nothing is happening",
                .Riding = "Never tenses up",
                .OnBack = "The calmest gun in the set, because he hasn't quite "
                          "registered that it's real",
                .Unread = "You can't tell what he's registered and what he hasn't, and "
                          "it matters which",
                .Unprompted = "You can tell when the calm is real and when he's "
                              "holding it together, which nobody else can",
            },
        .CostOfLoss = "He has a basketball hoop. After Omar nobody plays. It stands "
                      "there and nobody uses it.",
        .Activity = "at the hoop",
        .Tie = Joshua,
    },
    Member{
        .ID = Joshua,
        .Name = "Joshua",
        .Cap =
            Capability{
                .Name = "Doesn't flinch",
                .Riding = "Goes where you point without asking",
                .OnBack = "Doesn't look away",
                .Unread = "There's no version of this where you don't know him",
                .Unprompted = "He says what he's thinking without being asked, because "
                              "he always has",
            },
        .CostOfLoss = "He lives with Jamal. After Joshua, the house Jamal comes back "
                      "to is empty.",
        .Activity = "at home, because you live there",
        .Tie = Omar,
    },
};

bool MemberByID(const MemberID& id, Member& out) {
  for (const auto& m : TheSet) {
    if (m.ID == id) {
      out = m;
      return true;
    }
  }
  return false;
}

}  // namespace campaign
