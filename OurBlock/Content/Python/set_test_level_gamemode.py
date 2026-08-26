# TestLevel.umap was created before AGrayBoxGameMode existed, and its WorldSettings'
# GameMode Override takes priority over DefaultEngine.ini's GlobalDefaultGameMode
# regardless of what the project-wide setting says. This sets it directly on the level
# instead of relying on the ini alone.
#
# Run headlessly with the editor closed:
#   UnrealEditor-Cmd.exe OurBlock.uproject -ExecutePythonScript=Content/Python/set_test_level_gamemode.py -unattended -nopause -nullrhi
import unreal

LEVEL_PATH = "/Game/Maps/TestLevel"

level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
level_subsystem.load_level(LEVEL_PATH)

world = unreal.EditorLevelLibrary.get_editor_world()
world_settings = world.get_world_settings()
world_settings.set_editor_property("DefaultGameMode", unreal.GrayBoxGameMode)

level_subsystem.save_current_level()
unreal.log("set_test_level_gamemode: done")
