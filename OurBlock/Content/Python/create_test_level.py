# One-off script that created Content/Maps/TestLevel.umap - a flat plane and a
# PlayerStart, nothing else. Not the game; just enough for PIE/-game to have a level to
# load at all, which UCampaignSubsystem needs before it can initialize for real
# (GameInstanceSubsystem::Initialize only runs once a GameInstance exists).
#
# Run headlessly with the editor closed:
#   UnrealEditor-Cmd.exe OurBlock.uproject -ExecutePythonScript=Content/Python/create_test_level.py -unattended -nopause -nullrhi
#
# Kept here rather than discarded so recreating or rebuilding on the level is
# reproducible instead of undocumented manual editor clicking.
import unreal

LEVEL_PATH = "/Game/Maps/TestLevel"

level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

level_subsystem.new_level(LEVEL_PATH)

floor = actor_subsystem.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(0, 0, 0))
floor_mesh = unreal.load_asset("/Engine/BasicShapes/Plane")
floor.static_mesh_component.set_static_mesh(floor_mesh)
floor.set_actor_scale3d(unreal.Vector(20, 20, 1))
floor.set_actor_label("Floor")

player_start = actor_subsystem.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(0, 0, 100))
player_start.set_actor_label("PlayerStart")

level_subsystem.save_current_level()

unreal.log("create_test_level: done")
