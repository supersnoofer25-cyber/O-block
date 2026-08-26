# PlayerStart and ACompanionStandIn were both placed at (0, 0, 100) - the player spawns
# directly overlapping the companion's solid cube collision, which is why movement
# looked broken (the character controller was jammed against it from the first frame).
# Moves PlayerStart back and turns it to face the companion instead.
#
# Run headlessly with the editor closed:
#   UnrealEditor-Cmd.exe OurBlock.uproject -ExecutePythonScript=Content/Python/move_playerstart.py -unattended -nopause -nullrhi
import unreal

LEVEL_PATH = "/Game/Maps/TestLevel"

level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

level_subsystem.load_level(LEVEL_PATH)

for actor in actor_subsystem.get_all_level_actors():
    if isinstance(actor, unreal.PlayerStart):
        actor_subsystem.destroy_actor(actor)

# Yaw 90 faces +Y, toward the companion and the threats spread around it.
new_start = actor_subsystem.spawn_actor_from_class(
    unreal.PlayerStart, unreal.Vector(0, -800, 100), unreal.Rotator(0, 90, 0)
)
new_start.set_actor_label("PlayerStart")

level_subsystem.save_current_level()
unreal.log("move_playerstart: done")
