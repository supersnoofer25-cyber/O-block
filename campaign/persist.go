package campaign

import (
	"encoding/json"
	"os"
	"path/filepath"
)

// SlotName is the one save file. There is exactly one, and it is overwritten.
//
// ADR 0002 makes checkpointing autosave-only, because manual saves make save-scumming
// trivial and therefore make permanence optional. This package offers no way to name a
// slot, list slots, or restore an earlier one — story 27 is enforced by the absence of
// an API rather than by a rule someone has to remember.
const SlotName = "campaign.json"

// Save writes the campaign to its single slot, replacing what was there.
//
// Called by the engine after every decision, never by the player. The write is atomic
// via a temporary file and rename, so a crash mid-write cannot leave a half-written
// campaign — losing a whole campaign to a torn file would be a far worse outcome than
// any death in it.
func Save(dir string, s State) error {
	if err := os.MkdirAll(dir, 0o755); err != nil {
		return err
	}
	b, err := json.MarshalIndent(s, "", "  ")
	if err != nil {
		return err
	}
	final := filepath.Join(dir, SlotName)
	tmp := final + ".tmp"
	if err := os.WriteFile(tmp, b, 0o644); err != nil {
		return err
	}
	return os.Rename(tmp, final)
}

// Load reads the campaign back, resuming exactly where it stopped (story 26).
func Load(dir string) (State, error) {
	b, err := os.ReadFile(filepath.Join(dir, SlotName))
	if err != nil {
		return State{}, err
	}
	var s State
	if err := json.Unmarshal(b, &s); err != nil {
		return State{}, err
	}
	return s, nil
}
