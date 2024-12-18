// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LabInteractInputKey.generated.h"

class UInputAction;

UENUM(BlueprintType)
enum class ELabInteractionType : uint8
{
	InteractionType_Press	UMETA(DisplayName="Press"),
	InteractionType_Hold	UMETA(DisplayName="Hold")
};

UCLASS(Blueprintable, BlueprintType)
class LABINTERACTION_API ULabInteractInputKey : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TObjectPtr<UInputAction> InputAction;
};


UCLASS(BlueprintType)
class LABINTERACTION_API ULabInteractInputKeyInstance : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FText DisplayText = FText::FromString("Placeholder Text");
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULabInteractInputKey> InputKey = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	ELabInteractionType InteractionType = ELabInteractionType::InteractionType_Press;
	
	UPROPERTY(BlueprintReadOnly)
	float InteractionDuration = 1.f;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE FText& GetDisplayText() { return DisplayText; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE UInputAction* GetInputAction() const
	{
		if (InputKey == nullptr) return nullptr;
		return InputKey->InputAction;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ELabInteractionType GetInteractionType() const { return InteractionType; }
};
