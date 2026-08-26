# Populates Content/Maps/TestLevel.umap with one companion stand-in and three
# ThreatActors around it, so a human can actually press Play and judge whether denying
# a threat before it fires feels meaningfully different from not - the actual point of
# gray-boxing ADR 0015's danger tally, which no automation test can answer.
#
# Run headlessly with the editor closed:
#   UnrealEditor-Cmd.exe OurBlock.uproject -ExecutePythonScript=Content/Python/populate_test_encounter.py -unattended -nopause -nullrhi
import unreal

LEVEL_PATH = "/Game/Maps/TestLevel"

level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

level_subsystem.load_level(LEVEL_PATH)

companion = actor_subsystem.spawn_actor_from_class(unreal.CompanionStandIn, unreal.Vector(0, 0, 100))
companion.set_actor_label("Companion")

threat_positions = [
    unreal.Vector(600, 0, 100),
    unreal.Vector(0, 600, 100),
    unreal.Vector(-450, -350, 100),
]

companion_tally = companion.get_editor_property("Tally")

for i, pos in enumerate(threat_positions):
    threat = actor_subsystem.spawn_actor_from_class(unreal.ThreatActor, pos)
    threat.set_actor_label("Threat%d" % (i + 1))
    threat.set_editor_property("Target", companion_tally)

level_subsystem.save_current_level()
unreal.log("populate_test_encounter: done")
