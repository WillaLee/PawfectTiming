# Training Progress Blueprint Usage

## Add the Component
- Add `TrainingProgressComponent` to the dog actor or to a training flow actor in Blueprint.

## Initialize
- Call `InitializeProgress()` on BeginPlay if you want an explicit reset.
- Default result:
  - current stage = `Sit`
  - `Sit` is unlocked
  - `Lay` is locked

## Read State
- `GetCurrentStage()` returns the active stage.
- `IsUnlocked(ECueType)` checks whether `Sit` or `Lay` is available for progression. `Come` remains in `ECueType`, but this component does not use it for stage flow.
- `UnlockedMask` can be read in Blueprint for simple debug UI if needed.
- `bAllComplete` becomes `true` after the `Lay` stage is cleared.

## Drive Progression
- Call `StartStage(ECueType)` to switch to an unlocked stage.
- Call `AdvanceStage()` after a stage is successfully cleared.
- Expected unlock flow:
  - `Sit` cleared -> unlock `Lay`, move to `Lay`
  - `Lay` cleared -> no further stage, `bAllComplete = true`

## Blueprint Events
- Bind to `OnStageChanged` to update UI, prompts, or stage logic.
- Bind to `OnSkillUnlocked` to show unlock feedback or enable input buttons.
- Bind to `OnAllCompleted` to react when `Lay` is cleared and the full progress sequence is done.

## Suggested Integration
- Use `TrainingProgressComponent` to decide the active stage.
- Use `DogTrainingComponent` to evaluate whether the clicker press was correct for the current cue/state.
- Call `DogTrainingComponent.StartLevel(CurrentStage)` when a new stage begins so `TargetBehavior`, `Proficiency`, and one-shot completion flags are reset cleanly.
- `DogTrainingComponent.SetCue()` starts a cue TTL attempt window using `CueDuration`; when it expires, `CurrentCue` is cleared back to `None`, but the later click judgement can still succeed if a valid pose-triggered click window was already opened.
- Use `DogTrainingComponent.SampleNextDogState()` to sample whether the dog performs the current target behavior based on the current learning probability.
- Use `DogTrainingComponent.GetProficiencyNormalized()` and `DogTrainingComponent.GetLearningProbability()` to drive progress bars or learning UI.
- A click only succeeds during the separate pose-triggered click window:
  - when the dog enters `Sitting`, a click window opens only if the most recent cue was `Sit` and the pose was reached within `MaxCueToPoseDelay`
  - when the dog enters `Laying`, the same rule applies for `Lay`
  - this prevents very late random poses from counting as valid click targets
- Unlocked non-target cues can still be executed as performance actions, but only the current stage target updates `Proficiency` and learning probabilities.
- Use `ShouldTriggerIdleVariant()` and `PickIdleVariantIndex()` to drive idle-loop break variants when there is no active cue and the loop state is idle.
- Blueprint should gate decision sampling with `IsTransitionLocked()` instead of maintaining a separate transition-lock flag.
- Use `ShouldAutoReturnToIdle(OutReturnState)` or bind `OnRequestDogState` to auto-exit long holds:
  - `Sitting` -> `StandUp`
  - `Laying` -> `LayEnd`
- For cue/debug wiring, call `SetCue()` and `OnClickerPressed()` on the same `DogTrainingComponent` instance. Prefer resolving the component from the dog actor owner instead of caching a stale reference. Use `GetDebugInstanceId()` plus the built-in `this=%p` logs to verify Blueprint is talking to the same component instance.
- After a successful evaluation, call `AdvanceStage()` from Blueprint.
- Bind `DogTrainingComponent.OnTrainingResult` to react to every clicker evaluation.
- Bind `DogTrainingComponent.OnLevelCompleted` to react when a stage first reaches full completion.
- Bind `DogTrainingComponent.OnCueChanged` if HUD needs to reflect the active cue immediately and when it expires.
