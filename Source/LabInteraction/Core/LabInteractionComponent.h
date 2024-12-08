// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include "LabInteractionComponent.generated.h"


class ULabInteractableInterface;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LABINTERACTION_API ULabInteractionComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULabInteractionComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	TEnumAsByte<ECollisionChannel> DetectionChannel;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float DetectionFrequency = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float DetectionRadius = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bShowDebugTraces = false;
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetDetectionActive(const bool bNewActive);
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractionActive(bool bNewActive);
	
private:
	bool bDetectionActive = false;
	float LastUpdateTime = 0.f;
	
	UPROPERTY(ReplicatedUsing = OnRep_bInteractionActive)
	bool bInteractionActive = false;
	
	UPROPERTY(ReplicatedUsing = OnRep_FocusedInteractableActor)
	TObjectPtr<AActor> FocusedInteractableActor;
	
	UFUNCTION()
	void UpdateTraceInteractable(const float DeltaTime);

	UFUNCTION()
	EDrawDebugTrace::Type GetDrawDebugType() const;
	
	UFUNCTION()
	AActor* PerformTrace() const;
	
	UFUNCTION()
	AActor* FindNearestInteractable(const FVector& CurrentLocation, const TArray<FHitResult>& HitResults) const;

	UFUNCTION()
	void UpdateFocusedInteractable(AActor* InteractableActor);

	UFUNCTION()
	void OnRep_bInteractionActive();
	
	UFUNCTION()
	void OnRep_FocusedInteractableActor();
};
