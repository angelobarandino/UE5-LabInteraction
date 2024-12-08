// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LabInteractInput.generated.h"

class UInputAction;
/**
 * 
 */
UCLASS(BlueprintType)
class LABINTERACTION_API ULabInteractInput : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintGetter, Category = "LabInteraction Input")
	FORCEINLINE UInputAction* GetInputAction() const { return InputAction; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LabInteraction Input", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> InputAction;
};
