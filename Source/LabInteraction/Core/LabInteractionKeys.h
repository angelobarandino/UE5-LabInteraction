#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "LabInteractionKeys.generated.h"

class ULabInteractInputKey;

UENUM(BlueprintType)
enum class ELabInteractionType : uint8
{
	InteractionType_Press	UMETA(DisplayName="Press"),
	InteractionType_Hold	UMETA(DisplayName="Hold")
};

USTRUCT(BlueprintType)
struct FLabInteractInputTemplate
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Interaction")
	TSoftObjectPtr<ULabInteractInputKey> InputKey = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Interaction")
	FText DisplayText = FText::FromString("Placeholder Text");

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Interaction")
	ELabInteractionType InteractionType = ELabInteractionType::InteractionType_Press;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Interaction", meta = (EditCondition = "InteractionType == ELabInteractionType::InteractionType_Hold", EditConditionHides))
	float InteractionDuration = 1.f;
};

USTRUCT(BlueprintType)
struct FLabInteractionKeys : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Interaction")
	TArray<FLabInteractInputTemplate> InputKeys;
};

USTRUCT(BlueprintType)
struct FLabInteractableData
{
	GENERATED_BODY()

	FLabInteractableData() {}
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Interaction")
	FText DisplayText = FText::FromString("Placeholder Text");

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Interaction")
	FLabInteractionKeys InteractionData;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Interaction")
	TObjectPtr<UObject> CustomData = nullptr;
};