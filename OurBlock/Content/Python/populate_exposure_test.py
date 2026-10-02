# Populates TestLevel with a second encounter, this one for the "player rides, companion
# shoots" seat (ADR 0015's Exposure denial mode) rather than the click-to-deny cluster
# populate_test_encounter.py already placed at the origin. Companion.Tally isn't wired
# here the way the origin cluster's is - the companion for this seat is spawned onto the
# bike at runtime by AGrayBoxGameMode (bPlayerRides == true), so it doesn't exist yet at
# level-edit time. AGrayBoxGameMode::WireUnaimedExposureThreats wires any Exposure threat
# with no Target once that companion exists, which is why Target is left unset below.
#
# Placed well clear of the origin cluster (which stays in the level, inert in this mode -
# its Aimed threats still fire on their own fixed timer, targeting the old free-standing
# companion, but nothing reads that outcome) so the two test setups don't visually
# overlap. No cover geometry exists yet (ADR 0015's own worth-revisiting clause expects
# that as later work) - on the current flat floor, ExposureRange alone is what makes a
# threat's sightline break, not route or a wall, which is a real gap in what this can
# actually test until level geometry exists.
#
# Run headlessly with the editor closed:
#   UnrealEditor-Cmd.exe OurBlock.uproject -ExecutePythonScript=Content/Python/populate_exposure_test.py -unattended -nopause -nullrhi
import unreal

LEVEL_PATH = "/Game/Maps/TestLevel"
TIME_TO_FIRE = 5.0  # matches set_fuse_timing.py's current tuning value for the other seat

level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

level_subsystem.load_level(LEVEL_PATH)

# A rough driving line running +Y from PlayerStart (0, -800, 100), past the origin
# cluster, into open floor - offsets are perpendicular distances a driver passes at,
# not literal placements on the route itself.
threat_positions = [
    unreal.Vector(600, 1400, 100),
    unreal.Vector(-700, 1900, 100),
    unreal.Vector(1300, 2400, 100),  # near ExposureRange's default (1500) - tests the cutoff
]

for i, pos in enumerate(threat_positions):
    threat = actor_subsystem.spawn_actor_from_class(unreal.ThreatActor, pos)
    threat.set_actor_label("ExposureThreat%d" % (i + 1))
    threat.set_editor_property("DenialMode", unreal.ThreatDenial.EXPOSURE)
    threat.set_editor_property("TimeToFire", TIME_TO_FIRE)

level_subsystem.save_current_level()
unreal.log("populate_exposure_test: placed %d Exposure-mode threats" % len(threat_positions))
