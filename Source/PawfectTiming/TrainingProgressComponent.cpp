#include "TrainingProgressComponent.h"

UTrainingProgressComponent::UTrainingProgressComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTrainingProgressComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeProgress();
}

void UTrainingProgressComponent::InitializeProgress()
{
	UnlockedMask = 0;
	bAllComplete = false;
	UnlockSkill(ECueType::Sit);
	CurrentStage = ECueType::Sit;
	OnStageChanged.Broadcast(CurrentStage);
}

ECueType UTrainingProgressComponent::GetCurrentStage() const
{
	return CurrentStage;
}

bool UTrainingProgressComponent::IsUnlocked(ECueType Skill) const
{
	const int32 SkillBit = GetSkillBit(Skill);
	return SkillBit != 0 && (UnlockedMask & SkillBit) != 0;
}

void UTrainingProgressComponent::AdvanceStage()
{
	const ECueType NextStage = GetNextStage(CurrentStage);

	if (NextStage == ECueType::None)
	{
		if (CurrentStage == ECueType::Lay && !bAllComplete)
		{
			bAllComplete = true;
			OnAllCompleted.Broadcast();
		}

		return;
	}

	if (!IsUnlocked(NextStage))
	{
		UnlockSkill(NextStage);
	}

	StartStage(NextStage);
}

void UTrainingProgressComponent::StartStage(ECueType Stage)
{
	if (!IsTrainingStage(Stage) || !IsUnlocked(Stage) || CurrentStage == Stage)
	{
		return;
	}

	CurrentStage = Stage;
	OnStageChanged.Broadcast(CurrentStage);
}

int32 UTrainingProgressComponent::GetSkillBit(ECueType Skill)
{
	switch (Skill)
	{
	case ECueType::Come:
		return 1 << 0;
	case ECueType::Sit:
		return 1 << 1;
	case ECueType::Lay:
		return 1 << 2;
	case ECueType::None:
	default:
		return 0;
	}
}

ECueType UTrainingProgressComponent::GetNextStage(ECueType Stage)
{
	switch (Stage)
	{
	case ECueType::Sit:
		return ECueType::Lay;
	case ECueType::Lay:
	case ECueType::Come:
	case ECueType::None:
	default:
		return ECueType::None;
	}
}

bool UTrainingProgressComponent::IsTrainingStage(ECueType Stage)
{
	return Stage == ECueType::Sit || Stage == ECueType::Lay;
}

void UTrainingProgressComponent::UnlockSkill(ECueType Skill)
{
	const int32 SkillBit = GetSkillBit(Skill);

	if (SkillBit == 0 || (UnlockedMask & SkillBit) != 0)
	{
		return;
	}

	UnlockedMask |= SkillBit;
	OnSkillUnlocked.Broadcast(Skill);
}
