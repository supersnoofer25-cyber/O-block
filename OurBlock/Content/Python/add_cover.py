# Adds cover to TestLevel's exposure route. The scripted drive (AScriptedDrive) showed
# that on a flat floor the riding seat has no real decision in it: at full speed the
# bike crosses a threat's whole range in ~2.5s, half its 5s fuse, so keeping moving is
# always safe and only stopping is ever punished. ADR 0015's "never give it the angle"
# needs something to hide behind - that's what these are.
#
# One wall per Exposure threat (populate_exposure_test.py's (600,1400), (-700,1900),
# (1300,2400)), each standing between its threat and part of the X=300 lane the
# scripted drive uses. The layout is deliberately legible rather than clever, so a
# scripted run can check it:
#   - (300,1900) is hidden from all three - the "hide" spot. Each threat's line to it
#     crosses that threat's wall.
#   - (300,600) is in the open, in range of the (600,1400) threat only - the
#     "exposed" spot. Its line passes below that wall's south end.
# Everything else is for a human to find by playing.
#
# 300 tall, well over the sightline: threats sit at Z=100 and the companion rides at
# roughly Z=160, so nothing short of a wall this high reliably breaks the trace. The
# engine cube is 100 units per side centred on its pivot, so scale 3 on Z centred at
# Z=150 stands it on the floor (Z=0). The cube's default collision blocks the
# Visibility channel AThreatActor's sightline trace uses - and blocks the bike, which
# is the point: real cover is also something you can drive into.
#
# Idempotent: removes any earlier Cover* actors before placing these, so it can be
# re-run after tweaking positions without stacking duplicates.
#
# Run headlessly with the editor closed. The script path must be absolute - a relative
# one resolves against the engine's Binaries/Win64 directory, not the project, and fails
# with "Could not load Python file":
#   UnrealEditor-Cmd.exe OurBlock.uproject -ExecutePythonScript=<repo>/OurBlock/Content/Python/add_cover.py -unattended -nopause -nullrhi
import unreal

LEVEL_PATH = "/Game/Maps/TestLevel"
HEIGHT = 300.0
THICKNESS = 50.0

# (label, centre x, centre y, length along Y) - all walls run north-south, parallel to
# the lane, so the lane itself stays clear for the bike.
WALLS = [
    ("CoverEast1", 450.0, 1450.0, 700.0),   # y 1100..1800, shields (600,1400)
    ("CoverWest", -450.0, 1900.0, 500.0),   # y 1650..2150, shields (-700,1900)
    ("CoverEast2", 1050.0, 2400.0, 500.0),  # y 2150..2650, shields (1300,2400)
]

level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

level_subsystem.load_level(LEVEL_PATH)

for actor in actor_subsystem.get_all_level_actors():
    if actor.get_actor_label().startswith("Cover"):
        unreal.log("add_cover: removing old %s" % actor.get_actor_label())
        actor_subsystem.destroy_actor(actor)

cube = unreal.load_asset("/Engine/BasicShapes/Cube")
for label, x, y, length in WALLS:
    wall = actor_subsystem.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(x, y, HEIGHT / 2))
    wall.static_mesh_component.set_static_mesh(cube)
    wall.set_actor_scale3d(unreal.Vector(THICKNESS / 100, length / 100, HEIGHT / 100))
    wall.set_actor_label(label)
    unreal.log("add_cover: placed %s at (%.0f, %.0f), %.0f long" % (label, x, y, length))

level_subsystem.save_current_level()
unreal.log("add_cover: done")
