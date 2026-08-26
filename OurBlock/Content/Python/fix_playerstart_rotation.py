# move_playerstart.py passed unreal.Rotator(0, 90, 0) intending (Pitch, Yaw, Roll), but
# the Python constructor's actual positional order is (Roll, Pitch, Yaw) - confirmed by
# check_playerstart_rotation.py reading back pitch=90, yaw=0 from the saved level. That
# set the character looking straight up at the sky instead of facing the companion, and
# it stayed invisible until the mouselook-pitch fix made pitch actually affect the
# camera at all - before that, yaw (stuck at its default, facing world-East) was the
# only thing wrong for looking, which happened to go unnoticed.
#
# Run headlessly with the editor closed:
#   UnrealEditor-Cmd.exe OurBlock.uproject -ExecutePythonScript=Content/Python/fix_playerstart_rotation.py -unattended -nopause -nullrhi
import unreal

LEVEL_PATH = "/Game/Maps/TestLevel"

level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

level_subsystem.load_level(LEVEL_PATH)

for actor in actor_subsystem.get_all_level_actors():
    if isinstance(actor, unreal.PlayerStart):
        actor.set_actor_rotation(unreal.Rotator(0, 0, 90), False)  # (Roll, Pitch, Yaw)
        unreal.log("PlayerStart rotation set to %s" % (actor.get_actor_rotation(),))

level_subsystem.save_current_level()
unreal.log("fix_playerstart_rotation: done")
