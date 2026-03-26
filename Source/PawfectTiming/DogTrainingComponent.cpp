#include "DogTrainingComponent.h"

UDogTrainingComponent::UDogTrainingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDogTrainingComponent::BeginPlay()
{
	Super::BeginPlay();
	Proficiency = FMath::Clamp(Proficiency, 0, 100);
	bLevelComplete = Proficiency >= 100;
	bHasBroadcastLevelCompleted = bLevelComplete;
	RefreshTrainingWindow();
}

void UDogTrainingComponent::SetCue(ECueType NewCue)
{
	CurrentCue = NewCue;
	RefreshTrainingWindow();
}

bool UDogTrainingComponent::OnClickerPressed()
{
	RefreshTrainingWindow();

	const bool bWasLevelComplete = bLevelComplete;
	const bool bWasSuccess = CurrentCue == TargetBehavior && bInCorrectWindow;

	if (bWasSuccess)
	{
		Proficiency = FMath::Min(Proficiency + 10, 100);
	}
	else
	{
		Proficiency = FMath::Max(Proficiency - 2, 0);
	}

	bLevelComplete = Proficiency >= 100;
	OnTrainingResult.Broadcast(bWasSuccess);

	if (!bWasLevelComplete && bLevelComplete && !bHasBroadcastLevelCompleted)
	{
		bHasBroadcastLevelCompleted = true;
		OnLevelCompleted.Broadcast(TargetBehavior);
	}

	UE_LOG(LogTemp, Warning, TEXT("Click: Proficiency=%d WasComplete=%d NowComplete=%d HasBroadcast=%d Success=%d"),
		Proficiency, bWasLevelComplete, bLevelComplete, bHasBroadcastLevelCompleted, bWasSuccess);

	return bWasSuccess;
}

void UDogTrainingComponent::AdvanceDogState(EDogState NewState)
{
	DogState = NewState;
	RefreshTrainingWindow();
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
	bInCorrectWindow = IsInCorrectWindowForCue(CurrentCue, DogState);
}

