# Sets the placed ThreatActors' TimeToFire. Playtesting so far: 10s felt like no
# tension at all, 3s felt too fast to react to - trying 5s as a middle point. Expect
# this value to keep moving until it actually feels right; that is the point of a
# gray-box (spec.md open question 2 - the real number can only come from playing it).
#
# Run headlessly with the editor closed:
#   UnrealEditor-Cmd.exe OurBlock.uproject -ExecutePythonScript=Content/Python/set_fuse_timing.py -unattended -nopause -nullrhi
import unreal

LEVEL_PATH = "/Game/Maps/TestLevel"
TIME_TO_FIRE = 5.0

level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

level_subsystem.load_level(LEVEL_PATH)

count = 0
for actor in actor_subsystem.get_all_level_actors():
    if isinstance(actor, unreal.ThreatActor):
        actor.set_editor_property("TimeToFire", TIME_TO_FIRE)
        count += 1

level_subsystem.save_current_level()
unreal.log("set_fuse_timing: set TimeToFire=%.1f on %d threats" % (TIME_TO_FIRE, count))
