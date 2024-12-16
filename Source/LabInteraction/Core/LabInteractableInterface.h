// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LabInteractionKeys.h"
#include "UObject/Interface.h"
#include "LabInteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class ULabInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LABINTERACTION_API ILabInteractableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UpdateFocus(bool bActive);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void GetInteractableData(const FName& CurrentKey, FLabInteractableData& InteractableData);

	UFUNCTION(BlueprintImplementableEvent)
	void Interact(const FName& Name, const ULabInteractionComponent* Interactor);
};
