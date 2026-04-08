#include "DogTrainingComponent.h"

UDogTrainingComponent::UDogTrainingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDogTrainingComponent::BeginPlay()
{
	Super::BeginPlay();
	Proficiency = FMath::Clamp(Proficiency, 0, 100);
	PSit = FMath::Clamp(PSit, 0.0f, 1.0f);
	PLay = FMath::Clamp(PLay, 0.0f, 1.0f);
	AlphaSuccess = FMath::Clamp(AlphaSuccess, 0.0f, 1.0f);
	MaxP = FMath::Clamp(MaxP, 0.0f, 1.0f);
	MisexecuteFrac = FMath::Clamp(MisexecuteFrac, 0.0f, 1.0f);
	PLearnedDefault = FMath::Clamp(PLearnedDefault, 0.0f, 1.0f);
	CueDuration = FMath::Max(0.0f, CueDuration);
	MinHoldSitting = FMath::Max(MinHoldSitting, 0.0f);
	MinHoldLaying = FMath::Max(MinHoldLaying, 0.0f);
	MinTimeSitDown = FMath::Max(MinTimeSitDown, 0.0f);
	MinTimeStandUp = FMath::Max(MinTimeStandUp, 0.0f);
	MinTimeLayDown = FMath::Max(MinTimeLayDown, 0.0f);
	MinTimeLayEnd = FMath::Max(MinTimeLayEnd, 0.0f);
	IdleVariantCount = FMath::Max(IdleVariantCount, 1);
	IdleVariantTriggerIntervalMin = FMath::Max(IdleVariantTriggerIntervalMin, 0.0f);
	IdleVariantTriggerIntervalMax = FMath::Max(IdleVariantTriggerIntervalMax, IdleVariantTriggerIntervalMin);
	IdleVariantTriggerProbability = FMath::Clamp(IdleVariantTriggerProbability, 0.0f, 1.0f);
	IdleVariantCooldown = FMath::Max(IdleVariantCooldown, 0.0f);
	bLevelComplete = Proficiency >= 100;
	bHasBroadcastLevelCompleted = bLevelComplete;
	CurrentDogState = DogState;
	TimeInState = 0.0f;
	PendingTargetState = EDogState::Invalid;
	PCorrect = GetCueExecutionProbabilityValue(TargetBehavior);
	PMisexecute = (1.0f - PCorrect) * MisexecuteFrac;
	ResetIdleVariantTriggerTimer();
	RefreshTrainingWindow();
}

void UDogTrainingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	IdleVariantTriggerTimeRemaining = FMath::Max(IdleVariantTriggerTimeRemaining - DeltaTime, 0.0f);
	IdleVariantCooldownTimeRemaining = FMath::Max(IdleVariantCooldownTimeRemaining - DeltaTime, 0.0f);
	TimeInState += DeltaTime;

	if (CurrentCue == ECueType::None || CueTimeRemaining <= 0.0f)
	{
		return;
	}

	CueTimeRemaining -= DeltaTime;

	if (CueTimeRemaining <= 0.0f)
	{
		CueTimeRemaining = 0.0f;
		CurrentCue = ECueType::None;
		bShouldAttempt = false;
		RefreshTrainingWindow();
		OnCueChanged.Broadcast(CurrentCue);
	}
}

void UDogTrainingComponent::StartLevel(ECueType NewTarget)
{
	TargetBehavior = NewTarget;
	Proficiency = 0;
	bLevelComplete = false;
	bInCorrectWindow = false;
	bLastClickWasTraining = false;
	bHasBroadcastLevelCompleted = false;
	bShouldAttempt = false;
	CurrentCue = ECueType::None;
	CueTimeRemaining = 0.0f;
	CurrentDogState = EDogState::Idle;
	DogState = EDogState::Idle;
	TimeInState = 0.0f;
	PendingTargetState = EDogState::Invalid;
	IdleVariantCooldownTimeRemaining = 0.0f;
	ResetIdleVariantTriggerTimer();
	RefreshTrainingWindow();
	OnCueChanged.Broadcast(CurrentCue);
}

void UDogTrainingComponent::SetCue(ECueType NewCue)
{
	CurrentCue = NewCue;
	CueTimeRemaining = CueDuration;
	bShouldAttempt = CurrentCue != ECueType::None;
	PCorrect = GetCueExecutionProbabilityValue(CurrentCue);
	PMisexecute = (1.0f - PCorrect) * MisexecuteFrac;
	RefreshTrainingWindow();
	OnCueChanged.Broadcast(CurrentCue);
}

EDogState UDogTrainingComponent::SampleNextDogState()
{
	const EDogState DesiredState = GetDesiredStateFromCue();
	return ResolveNextStateFromDesired(DesiredState);
}

bool UDogTrainingComponent::OnClickerPressed()
{
	RefreshTrainingWindow();

	const bool bWasLevelComplete = bLevelComplete;
	bLastClickWasTraining = CurrentCue == TargetBehavior;

	const bool bWasSuccess = CurrentCue != ECueType::None && bInCorrectWindow;

	if (bWasSuccess && bLastClickWasTraining)
	{
		if (float* LearningProbability = GetLearningProbabilityPtr())
		{
			*LearningProbability = FMath::Clamp(*LearningProbability + AlphaSuccess, 0.0f, MaxP);
		}

		Proficiency = FMath::Min(Proficiency + 20, 100);
	}

	bLevelComplete = Proficiency >= 100;
	OnTrainingResult.Broadcast(bWasSuccess);

	if (!bWasLevelComplete && bLevelComplete && !bHasBroadcastLevelCompleted)
	{
		bHasBroadcastLevelCompleted = true;
		OnLevelCompleted.Broadcast(TargetBehavior);
	}

	return bWasSuccess;
}

void UDogTrainingComponent::AdvanceDogState(EDogState NewState)
{
	CurrentDogState = NewState;
	DogState = NewState;
	TimeInState = 0.0f;
	RefreshTrainingWindow();
}

float UDogTrainingComponent::GetProficiencyNormalized() const
{
	return FMath::Clamp(static_cast<float>(Proficiency) / 100.0f, 0.0f, 1.0f);
}

float UDogTrainingComponent::GetLearningProbability() const
{
	return GetLearningProbabilityValue();
}

bool UDogTrainingComponent::ShouldTriggerIdleVariant()
{
	if (CurrentCue != ECueType::None || LoopState != EDogState::Idle || bIdleVariantLocked)
	{
		return false;
	}

	if (IdleVariantCooldownTimeRemaining > 0.0f || IdleVariantTriggerTimeRemaining > 0.0f)
	{
		return false;
	}

	ResetIdleVariantTriggerTimer();

	if (FMath::FRand() > IdleVariantTriggerProbability)
	{
		return false;
	}

	IdleVariantCooldownTimeRemaining = IdleVariantCooldown;
	return true;
}

int32 UDogTrainingComponent::PickIdleVariantIndex() const
{
	if (IdleVariantCount <= 1)
	{
		return 0;
	}

	return FMath::RandRange(0, IdleVariantCount - 1);
}

float* UDogTrainingComponent::GetLearningProbabilityPtr()
{
	switch (TargetBehavior)
	{
	case ECueType::Sit:
		return &PSit;
	case ECueType::Lay:
		return &PLay;
	case ECueType::Come:
	case ECueType::None:
	default:
		return nullptr;
	}
}

float UDogTrainingComponent::GetLearningProbabilityValue() const
{
	switch (TargetBehavior)
	{
	case ECueType::Sit:
		return PSit;
	case ECueType::Lay:
		return PLay;
	case ECueType::Come:
	case ECueType::None:
	default:
		return 0.0f;
	}
}

float UDogTrainingComponent::GetCueExecutionProbabilityValue(ECueType Cue) const
{
	switch (Cue)
	{
	case ECueType::Sit:
		return FMath::Clamp(PSit, 0.0f, 1.0f);
	case ECueType::Lay:
		return FMath::Clamp(PLay, 0.0f, 1.0f);
	case ECueType::Come:
	case ECueType::None:
	default:
		return 0.0f;
	}
}

EDogState UDogTrainingComponent::GetDesiredStateFromCue() const
{
	if (CurrentCue == ECueType::None)
	{
		return EDogState::Idle;
	}

	const float EffectiveCorrect = FMath::Clamp(GetCueExecutionProbabilityValue(CurrentCue), 0.0f, 1.0f);
	const float EffectiveMisexecute = FMath::Clamp((1.0f - EffectiveCorrect) * MisexecuteFrac, 0.0f, 1.0f - EffectiveCorrect);
	const float Roll = FMath::FRand();

	switch (CurrentCue)
	{
	case ECueType::Sit:
		if (Roll < EffectiveCorrect)
		{
			return EDogState::Sitting;
		}
		if (Roll < EffectiveCorrect + EffectiveMisexecute)
		{
			return EDogState::Laying;
		}
		return EDogState::Idle;
	case ECueType::Lay:
		if (Roll < EffectiveCorrect)
		{
			return EDogState::Laying;
		}
		if (Roll < EffectiveCorrect + EffectiveMisexecute)
		{
			return EDogState::Sitting;
		}
		return EDogState::Idle;
	case ECueType::Come:
	case ECueType::None:
	default:
		return EDogState::Idle;
	}
}

EDogState UDogTrainingComponent::ResolveNextStateFromDesired(EDogState DesiredState)
{
	bShouldAttempt = CurrentCue != ECueType::None;
	PCorrect = GetCueExecutionProbabilityValue(CurrentCue);
	PMisexecute = (1.0f - PCorrect) * MisexecuteFrac;

	if (CurrentDogState == EDogState::Sitting && TimeInState < MinHoldSitting)
	{
		return EDogState::Sitting;
	}

	if (CurrentDogState == EDogState::Laying && TimeInState < MinHoldLaying)
	{
		return EDogState::Laying;
	}

	if (CurrentDogState == EDogState::SitDown)
	{
		return TimeInState >= MinTimeSitDown ? EDogState::Sitting : EDogState::SitDown;
	}

	if (CurrentDogState == EDogState::LayDown)
	{
		return TimeInState >= MinTimeLayDown ? EDogState::Laying : EDogState::LayDown;
	}

	if (CurrentDogState == EDogState::LayEnd)
	{
		return TimeInState >= MinTimeLayEnd ? EDogState::Idle : EDogState::LayEnd;
	}

	if (CurrentDogState == EDogState::StandUp)
	{
		if (TimeInState < MinTimeStandUp)
		{
			return EDogState::StandUp;
		}

		if (PendingTargetState == EDogState::Sitting)
		{
			PendingTargetState = EDogState::Invalid;
			return EDogState::SitDown;
		}

		if (PendingTargetState == EDogState::Laying)
		{
			PendingTargetState = EDogState::Invalid;
			return EDogState::LayDown;
		}

		return EDogState::Idle;
	}

	if (DesiredState == EDogState::Sitting)
	{
		if (CurrentDogState == EDogState::Laying)
		{
			PendingTargetState = EDogState::Sitting;
			return EDogState::StandUp;
		}

		if (CurrentDogState == EDogState::Idle)
		{
			return EDogState::SitDown;
		}

		return EDogState::Sitting;
	}

	if (DesiredState == EDogState::Laying)
	{
		if (CurrentDogState == EDogState::Sitting)
		{
			PendingTargetState = EDogState::Laying;
			return EDogState::StandUp;
		}

		if (CurrentDogState == EDogState::Idle)
		{
			return EDogState::LayDown;
		}

		return EDogState::Laying;
	}

	if (CurrentDogState == EDogState::Sitting)
	{
		return EDogState::StandUp;
	}

	if (CurrentDogState == EDogState::Laying)
	{
		return EDogState::LayEnd;
	}

	PendingTargetState = EDogState::Invalid;
	return EDogState::Idle;
}

void UDogTrainingComponent::ResetIdleVariantTriggerTimer()
{
	const float MaxInterval = FMath::Max(IdleVariantTriggerIntervalMax, IdleVariantTriggerIntervalMin);
	IdleVariantTriggerTimeRemaining = FMath::FRandRange(IdleVariantTriggerIntervalMin, MaxInterval);
}

bool UDogTrainingComponent::IsInCorrectWindowForCue(ECueType Cue, EDogState State) const
{
	switch (Cue)
	{
	case ECueType::Come:
		return false;
	case ECueType::Sit:
		return State == EDogState::Sitting;
	case ECueType::Lay:
		return State == EDogState::Laying;
	case ECueType::None:
	default:
		return false;
	}
}

void UDogTrainingComponent::RefreshTrainingWindow()
{
	bInCorrectWindow = IsInCorrectWindowForCue(CurrentCue, CurrentDogState);
}

