﻿#pragma once

#include "CoreMinimal.h"
#include "LabInteractInputKey.h"
#include "Engine/DataTable.h"
#include "LabInteraction/LabInteraction.h"
#include "LabInteractionKeys.generated.h"

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

	FLabInteractableData() : Key("Default") {}
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Interaction")
	FName Key;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Interaction")
	FText DisplayText = FText::FromString("Placeholder Text");

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Interaction")
	FDataTableRowHandle InteractionKeysRowHandle = FDataTableRowHandle();
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Interaction")
	TWeakObjectPtr<UObject> CustomData = nullptr;

	FORCEINLINE TArray<FLabInteractInputTemplate> GetInputKeys() const
	{
		if (InteractionKeysRowHandle.IsNull())
		{
			UE_LOG(LogLabInteraction, Warning, TEXT("GetInputKeys: Failed to retrieve row from InteractionKeysRowHandle, there is no valid data."));
			return TArray<FLabInteractInputTemplate>();
		}
		
		if (CachedInteractionKeys.IsSet())
		{
			return CachedInteractionKeys.GetValue()->InputKeys;
		} 
		
		CachedInteractionKeys = InteractionKeysRowHandle.GetRow<FLabInteractionKeys>("GetInputKeys: FLabInteractableData::GetInputKeys");
		if (!CachedInteractionKeys.IsSet())
		{
			UE_LOG(LogLabInteraction, Warning, TEXT("GetInputKeys: Failed to retrieve FLabInteractableData. CachedInteractionKeys.IsSet() function returned false."));
			return TArray<FLabInteractInputTemplate>();
		}

		return CachedInteractionKeys.GetValue()->InputKeys;
	}

private:
    mutable TOptional<FLabInteractionKeys*> CachedInteractionKeys;
};