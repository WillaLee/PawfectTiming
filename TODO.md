# PawfectTiming TODO

## Phase 1: Core Training Data
- Define stage progression for `Come -> Sit -> Lay`
- Add stage/result enums or structs in C++
- Track unlocked skills independently from the active cue
- Store per-stage status: locked, active, cleared, missed

## Phase 2: Gameplay Logic
- Implement single-map training flow controller
- Set starting stage to `Come`
- Advance to the next stage only after `Success`
- Record `Miss` without unlocking the next skill
- Mark the full MVP sequence complete after `Lay` success

## Phase 3: Dog Behavior Integration
- Support a `Come` behavior state or completion condition in C++
- Keep `Sit` and `Lay` validation windows wired to animation/state changes
- Expose stage state to Blueprint and AnimBP
- Let the dog actor or AnimBP notify C++ when the dog enters the relevant states

## Phase 4: Input
- Wire `1` to `Come`
- Wire `2` to `Sit`
- Wire `3` to `Lay`
- Wire left mouse button to `Clicker`
- Prevent locked skills from firing as valid stage actions

## Phase 5: Feedback
- Show current stage on screen
- Show latest result: `Success` or `Miss`
- Show which skills are unlocked
- Show when the player has completed all three stages

## Phase 6: Validation
- Test success and miss paths for all three stages
- Test unlock order: `Come` -> `Sit` -> `Lay`
- Verify no logic depends on editing `Content/` assets
- Verify Blueprint/AnimBP can read the exposed training state
