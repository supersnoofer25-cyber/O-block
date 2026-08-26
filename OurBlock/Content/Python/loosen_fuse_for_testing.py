# Temporarily lengthens the already-placed ThreatActors' fuse from 3s to 10s, so we can
# confirm firing/denying actually works at all before dialing the timing back down to
# something that's meant to feel tense. Changing AThreatActor's C++ default wouldn't
# reliably reach these instances, since their TimeToFire was already saved into the
# level at spawn time.
#
# Run headlessly with the editor closed:
#   UnrealEditor-Cmd.exe OurBlock.uproject -ExecutePythonScript=Content/Python/loosen_fuse_for_testing.py -unattended -nopause -nullrhi
import unreal

LEVEL_PATH = "/Game/Maps/TestLevel"

level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

level_subsystem.load_level(LEVEL_PATH)

count = 0
for actor in actor_subsystem.get_all_level_actors():
    if isinstance(actor, unreal.ThreatActor):
        actor.set_editor_property("TimeToFire", 10.0)
        count += 1

level_subsystem.save_current_level()
unreal.log("loosen_fuse_for_testing: updated %d threats" % count)
