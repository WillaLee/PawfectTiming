#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DogTrainingComponent.generated.h"

UENUM(BlueprintType)
enum class EDogState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	SitDown UMETA(DisplayName = "SitDown"),
	Sitting UMETA(DisplayName = "Sitting"),
	LayDown UMETA(DisplayName = "LayDown"),
	Laying UMETA(DisplayName = "Laying"),
	StandUp UMETA(DisplayName = "StandUp"),
	LayEnd UMETA(DisplayName = "LayEnd"),
	Invalid UMETA(DisplayName = "Invalid")
};

UENUM(BlueprintType)
enum class ECueType : uint8
{
	None UMETA(DisplayName = "None"),
	Come UMETA(DisplayName = "Come"),
	Sit UMETA(DisplayName = "Sit"),
	Lay UMETA(DisplayName = "Lay")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrainingResult, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrainingLevelCompleted, ECueType, CompletedStage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCueChanged, ECueType, NewCue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestDogState, EDogState, NewState);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PAWFECTTIMING_API UDogTrainingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDogTrainingComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training")
	ECueType CurrentCue = ECueType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training")
	ECueType TargetBehavior = ECueType::Sit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training", meta = (ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"))
	int32 Proficiency = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Training")
	bool bInCorrectWindow = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Training")
	bool bLevelComplete = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Training")
	EDogState DogState = EDogState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Training")
	EDogState CurrentDogState = EDogState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Training")
	float TimeInState = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Training")
	EDogState PendingTargetState = EDogState::Invalid;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Training")
	bool bLastClickWasTraining = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Idle Variant")
	EDogState LoopState = EDogState::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Idle Variant")
	bool bIdleVariantLocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Idle Variant", meta = (ClampMin = "1", UIMin = "1"))
	int32 IdleVariantCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Idle Variant", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float IdleVariantTriggerIntervalMin = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Idle Variant", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float IdleVariantTriggerIntervalMax = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Idle Variant", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float IdleVariantTriggerProbability = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Idle Variant", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float IdleVariantCooldown = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cue", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float CueDuration = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cue", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float ClickWindowDuration = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cue", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MaxCueToPoseDelay = 3.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cue")
	ECueType LastIssuedCue = ECueType::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cue")
	float TimeSinceLastCue = 999.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cue")
	bool bClickWindowActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cue")
	float ClickWindowRemaining = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cue")
	ECueType AchievedBehavior = ECueType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Learning", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float PSit = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Learning", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float PLay = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Learning", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float AlphaSuccess = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Learning", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MaxP = 0.9f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Learning", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DecisionInterval = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Learning", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float PCorrect = 0.6f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Learning", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float PMisexecute = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Learning", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MisexecuteFrac = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Learning", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float PLearnedDefault = 0.9f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Timing", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MinHoldSitting = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Timing", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MinHoldLaying = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Timing", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MinTimeSitDown = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Timing", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MinTimeStandUp = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Timing", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MinTimeLayDown = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Timing", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MinTimeLayEnd = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State Timing", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float AutoReturnToIdleSeconds = 2.0f;

	UPROPERTY(BlueprintAssignable, Category = "Training")
	FOnTrainingResult OnTrainingResult;

	UPROPERTY(BlueprintAssignable, Category = "Training")
	FOnTrainingLevelCompleted OnLevelCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Cue")
	FOnCueChanged OnCueChanged;

	UPROPERTY(BlueprintAssignable, Category = "Training")
	FOnRequestDogState OnRequestDogState;

	UFUNCTION(BlueprintCallable, Category = "Training")
	void StartLevel(ECueType NewTarget);

	UFUNCTION(BlueprintCallable, Category = "Training")
	void SetCue(ECueType NewCue);

	UFUNCTION(BlueprintCallable, Category = "Training")
	EDogState SampleNextDogState();

	UFUNCTION(BlueprintCallable, Category = "Training")
	bool OnClickerPressed();

	UFUNCTION(BlueprintCallable, Category = "Training")
	void AdvanceDogState(EDogState NewState);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Training")
	float GetProficiencyNormalized() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Training")
	float GetLearningProbability() const;

	UFUNCTION(BlueprintPure, Category = "Debug")
	FString GetDebugInstanceId() const;

	UFUNCTION(BlueprintPure, Category = "Training")
	bool IsTransitionState(EDogState State) const;

	UFUNCTION(BlueprintPure, Category = "Training")
	bool IsTransitionLocked() const;

	UFUNCTION(BlueprintPure, Category = "Training")
	bool ShouldAutoReturnToIdle(EDogState& OutReturnState) const;

	UFUNCTION(BlueprintCallable, Category = "Idle Variant")
	bool ShouldTriggerIdleVariant();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Idle Variant")
	int32 PickIdleVariantIndex() const;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	bool bHasBroadcastLevelCompleted = false;
	bool bShouldAttempt = false;
	float CueTimeRemaining = 0.0f;
	float IdleVariantTriggerTimeRemaining = 0.0f;
	float IdleVariantCooldownTimeRemaining = 0.0f;
	bool bHasBroadcastAutoReturnRequest = false;
	float* GetLearningProbabilityPtr();
	float GetLearningProbabilityValue() const;
	float GetCueExecutionProbabilityValue(ECueType Cue) const;
	EDogState GetDesiredStateFromCue() const;
	EDogState ResolveNextStateFromDesired(EDogState DesiredState);
	bool IsCorrectPoseFor(ECueType Cue, EDogState State) const;
	bool IsInCorrectWindowForCue(ECueType Cue, EDogState State) const;
	void ResetIdleVariantTriggerTimer();
	void RefreshTrainingWindow();
};
