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
	StandUp UMETA(DisplayName = "StandUp")
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

	UPROPERTY(BlueprintAssignable, Category = "Training")
	FOnTrainingResult OnTrainingResult;

	UPROPERTY(BlueprintAssignable, Category = "Training")
	FOnTrainingLevelCompleted OnLevelCompleted;

	UFUNCTION(BlueprintCallable, Category = "Training")
	void SetCue(ECueType NewCue);

	UFUNCTION(BlueprintCallable, Category = "Training")
	bool OnClickerPressed();

	UFUNCTION(BlueprintCallable, Category = "Training")
	void AdvanceDogState(EDogState NewState);

protected:
	virtual void BeginPlay() override;

private:
	bool bHasBroadcastLevelCompleted = false;
	bool IsInCorrectWindowForCue(ECueType Cue, EDogState State) const;
	void RefreshTrainingWindow();
};
