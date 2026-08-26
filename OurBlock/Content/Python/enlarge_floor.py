# The floor was sized (2000x2000, extent 1000) for a standing/shooting encounter, where
# nobody needs to move far. It's too small for a bike with a proper chase camera:
# PlayerStart sits 800 units from center, leaving only 200 units of clearance before the
# edge, and the driver's camera (450 units behind the bike) exceeded that by 250 units,
# putting the camera past the floor's edge with void in the foreground. Scales the floor
# up 3x (extent 1000 -> 3000) rather than compromise the camera distance or move
# PlayerStart, since a bigger floor doesn't cost anything else in this gray-box.
#
# Run headlessly with the editor closed:
#   UnrealEditor-Cmd.exe OurBlock.uproject -ExecutePythonScript=Content/Python/enlarge_floor.py -unattended -nopause -nullrhi
import unreal

LEVEL_PATH = "/Game/Maps/TestLevel"

level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

level_subsystem.load_level(LEVEL_PATH)

for actor in actor_subsystem.get_all_level_actors():
    if actor.get_actor_label() == "Floor":
        scale = actor.get_actor_scale3d()
        new_scale = unreal.Vector(scale.x * 3, scale.y * 3, scale.z)
        actor.set_actor_scale3d(new_scale)
        unreal.log("Floor scale set to %s" % (new_scale,))

level_subsystem.save_current_level()
unreal.log("enlarge_floor: done")
