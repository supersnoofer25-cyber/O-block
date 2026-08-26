# Read-only diagnostic: prints the floor's actual world-space bounding box and the
# PlayerStart's location, so camera/seat placement can be based on real numbers instead
# of assumptions about the default engine plane's size.
#
# Run headlessly with the editor closed:
#   UnrealEditor-Cmd.exe OurBlock.uproject -ExecutePythonScript=Content/Python/check_floor_bounds.py -unattended -nopause -nullrhi
import unreal

LEVEL_PATH = "/Game/Maps/TestLevel"

level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

level_subsystem.load_level(LEVEL_PATH)

for actor in actor_subsystem.get_all_level_actors():
    if actor.get_actor_label() == "Floor":
        origin, extent = actor.get_actor_bounds(only_colliding_components=False)
        unreal.log("Floor origin=%s extent=%s location=%s" % (origin, extent, actor.get_actor_location()))
    if isinstance(actor, unreal.PlayerStart):
        unreal.log("PlayerStart location=%s" % (actor.get_actor_location(),))
