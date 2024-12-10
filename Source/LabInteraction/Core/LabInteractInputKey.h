// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LabInteractInputKey.generated.h"

class UInputAction;


UCLASS(Blueprintable, BlueprintType)
class LABINTERACTION_API ULabInteractInputKey : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TObjectPtr<UInputAction> InputAction;
};
