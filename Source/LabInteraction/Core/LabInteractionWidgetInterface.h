// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LabInteractionWidgetInterface.generated.h"

class ULabInteractionComponent;
// This class does not need to be modified.
UINTERFACE()
class ULabInteractionWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LABINTERACTION_API ILabInteractionWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintImplementableEvent)
	void OnInitInteractionWidget(ULabInteractionComponent* Interactor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UpdateDisplay(const FText& DisplayName, const TArray<FLabInteractInputTemplate>& InteractionKeys);
	
};
