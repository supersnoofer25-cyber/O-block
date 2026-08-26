# Read-only diagnostic: prints PlayerStart's actual saved transform so we can verify
# it's really facing the companion, rather than continue guessing about it.
#
# Run headlessly with the editor closed:
#   UnrealEditor-Cmd.exe OurBlock.uproject -ExecutePythonScript=Content/Python/check_playerstart_rotation.py -unattended -nopause -nullrhi
import unreal

LEVEL_PATH = "/Game/Maps/TestLevel"

level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

level_subsystem.load_level(LEVEL_PATH)

for actor in actor_subsystem.get_all_level_actors():
    if isinstance(actor, unreal.PlayerStart):
        loc = actor.get_actor_location()
        rot = actor.get_actor_rotation()
        unreal.log("PlayerStart location=%s rotation=%s" % (loc, rot))
    if isinstance(actor, unreal.CompanionStandIn):
        loc = actor.get_actor_location()
        unreal.log("CompanionStandIn location=%s" % (loc,))
