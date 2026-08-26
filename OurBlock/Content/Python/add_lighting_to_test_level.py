# TestLevel was created with a floor and a PlayerStart but no light source, which with
# Lumen GI enabled renders as an effectively black scene - nothing to see, nothing to
# aim at within a threat's fuse window. Adds a movable directional light (sun) and a
# sky light for ambient fill, neither requiring a lighting build since both are dynamic.
#
# Run headlessly with the editor closed:
#   UnrealEditor-Cmd.exe OurBlock.uproject -ExecutePythonScript=Content/Python/add_lighting_to_test_level.py -unattended -nopause -nullrhi
import unreal

LEVEL_PATH = "/Game/Maps/TestLevel"

level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

level_subsystem.load_level(LEVEL_PATH)

sun = actor_subsystem.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0, 0, 500))
sun.set_actor_rotation(unreal.Rotator(0, -45, -45), False)
sun.set_actor_label("Sun")
sun.get_editor_property("light_component").set_editor_property("mobility", unreal.ComponentMobility.MOVABLE)
sun.get_editor_property("light_component").set_editor_property("intensity", 5.0)

sky = actor_subsystem.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0, 0, 500))
sky.set_actor_label("SkyLight")
sky.get_editor_property("light_component").set_editor_property("mobility", unreal.ComponentMobility.MOVABLE)

level_subsystem.save_current_level()
unreal.log("add_lighting_to_test_level: done")
