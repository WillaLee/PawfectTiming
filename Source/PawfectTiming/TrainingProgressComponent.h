#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DogTrainingComponent.h"
#include "TrainingProgressComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrainingStageChanged, ECueType, NewStage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrainingSkillUnlocked, ECueType, Skill);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrainingAllCompleted);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PAWFECTTIMING_API UTrainingProgressComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTrainingProgressComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Training Progress")
	ECueType CurrentStage = ECueType::Sit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Training Progress")
	int32 UnlockedMask = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Training Progress")
	bool bAllComplete = false;

	UPROPERTY(BlueprintAssignable, Category = "Training Progress")
	FOnTrainingStageChanged OnStageChanged;

	UPROPERTY(BlueprintAssignable, Category = "Training Progress")
	FOnTrainingSkillUnlocked OnSkillUnlocked;

	UPROPERTY(BlueprintAssignable, Category = "Training Progress")
	FOnTrainingAllCompleted OnAllCompleted;

	UFUNCTION(BlueprintCallable, Category = "Training Progress")
	void InitializeProgress();

	UFUNCTION(BlueprintCallable, Category = "Training Progress")
	ECueType GetCurrentStage() const;

	UFUNCTION(BlueprintCallable, Category = "Training Progress")
	bool IsUnlocked(ECueType Skill) const;

	UFUNCTION(BlueprintCallable, Category = "Training Progress")
	void AdvanceStage();

	UFUNCTION(BlueprintCallable, Category = "Training Progress")
	void StartStage(ECueType Stage);

protected:
	virtual void BeginPlay() override;

private:
	static int32 GetSkillBit(ECueType Skill);
	static ECueType GetNextStage(ECueType Stage);
	static bool IsTrainingStage(ECueType Stage);
	void UnlockSkill(ECueType Skill);
};
