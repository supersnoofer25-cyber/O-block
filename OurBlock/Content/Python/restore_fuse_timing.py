# Reverts the placed ThreatActors' TimeToFire back to 3.0s, the actual gray-box value -
# loosen_fuse_for_testing.py bumped it to 10s only to isolate whether firing/denying
# worked at all, separate from time pressure. Now that's confirmed working, this puts
# the tension back.
#
# Run headlessly with the editor closed:
#   UnrealEditor-Cmd.exe OurBlock.uproject -ExecutePythonScript=Content/Python/restore_fuse_timing.py -unattended -nopause -nullrhi
import unreal

LEVEL_PATH = "/Game/Maps/TestLevel"

level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

level_subsystem.load_level(LEVEL_PATH)

count = 0
for actor in actor_subsystem.get_all_level_actors():
    if isinstance(actor, unreal.ThreatActor):
        actor.set_editor_property("TimeToFire", 3.0)
        count += 1

level_subsystem.save_current_level()
unreal.log("restore_fuse_timing: updated %d threats" % count)
