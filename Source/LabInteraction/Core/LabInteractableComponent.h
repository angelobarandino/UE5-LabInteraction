// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LabInteractableInterface.h"
#include "Components/WidgetComponent.h"
#include "LabInteractableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocusUpdate, bool, bActive);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LABINTERACTION_API ULabInteractableComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULabInteractableComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,  FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

public:

	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FOnFocusUpdate OnFocusUpdate;

private:
	UPROPERTY(ReplicatedUsing = "OnReplicated_bInteractionActive")
	bool bInteractionActive = true;

	UFUNCTION()
	void OnReplicated_bInteractionActive();

};
