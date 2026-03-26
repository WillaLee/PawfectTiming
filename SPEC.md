# PawfectTiming MVP Spec

## Scope
- Single playable map
- One dog training flow with three stages:
  - `Come`
  - `Sit`
  - `Lay`
- Each stage can result in:
  - `Success`
  - `Miss`
- Skills unlock progressively as the player clears earlier stages

## Constraints
- Do not modify any binary assets under `Content/`
- Implementation work is limited to:
  - `Source/`
  - `Config/`
  - `Docs/`
  - `Scripts/`

## Input
- `1` = `Come`
- `2` = `Sit`
- `3` = `Lay`
- Left mouse button = `Clicker`

## MVP Loop
1. Player enters the single training map.
2. The current stage defines the active training goal.
3. The player gives a cue with keyboard input.
4. The dog transitions into the matching behavior state.
5. The player presses the clicker during the valid success window.
6. The game records either `Success` or `Miss`.
7. Clearing a stage unlocks the next skill and advances the sequence.

## Stage Design
### Stage 1: Come
- Goal: dog comes to the player or target interaction zone
- Available cue: `Come`
- Result:
  - `Success` if clicker is pressed while the dog is in the valid arrive/come-complete state
  - `Miss` otherwise
- Unlocks: `Sit`

### Stage 2: Sit
- Goal: dog reaches and holds the sit state
- Available cues:
  - `Come`
  - `Sit`
- Result:
  - `Success` if clicker is pressed while the dog is in the valid `Sitting` window for the `Sit` cue
  - `Miss` otherwise
- Unlocks: `Lay`

### Stage 3: Lay
- Goal: dog reaches and holds the lay state
- Available cues:
  - `Come`
  - `Sit`
  - `Lay`
- Result:
  - `Success` if clicker is pressed while the dog is in the valid `Laying` window for the `Lay` cue
  - `Miss` otherwise
- Unlocks: stage sequence complete

## State / Result Model
- Training progression is stage-based, not free-form
- Each stage tracks:
  - current target behavior
  - whether the required skill is unlocked
  - most recent result: `Success` or `Miss`
- The player should not access later skills before they are unlocked

## Skill Unlock Rules
- `Come` is available from the start
- `Sit` unlocks after clearing the `Come` stage
- `Lay` unlocks after clearing the `Sit` stage
- Completing `Lay` marks the MVP training sequence complete

## Success / Miss Rules
- `Success`
  - the active cue matches the current stage requirement
  - the dog is inside that stage's valid behavior window
  - the player presses the clicker in that window
- `Miss`
  - wrong cue for the current stage
  - clicker pressed outside the valid window
  - dog not yet in the required state

## C++ / Blueprint Boundary
- Core training rules should live in C++
- Blueprints and AnimBP should consume exposed enums, properties, and callable functions
- Dog animation state changes can still be driven by Blueprint / AnimBP, but authoritative evaluation logic should remain in C++

## Future Extensions
- Multiple maps or difficulty variants
- Gesture-based cue input
- Voice-command cue input
- More detailed scoring than binary `Success` / `Miss`
- Replayable drills after all skills are unlocked
