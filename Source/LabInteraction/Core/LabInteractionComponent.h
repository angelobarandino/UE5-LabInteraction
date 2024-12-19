// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LabInteractionKeys.h"
#include "Components/ActorComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include "LabInteractionComponent.generated.h"


class ULabInteractInputKeyInstance;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHoldProgressUpdated, ULabInteractInputKey*, InputKey, float, Progress);

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

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHoldProgressUpdated OnHoldProgressUpdated;
	
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
	
	UFUNCTION(BlueprintCallable, Category = "Interaction", meta=(DeterminesOutputType=NewWidgetClass))
	UUserWidget* PushWidget(const TSubclassOf<UUserWidget> NewWidgetClass);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void PopWidget(UUserWidget*& OutActiveWidget);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void InteractionInput(ULabInteractInputKey* InputKey, const bool bPressed);

	UFUNCTION(Server, Reliable)
	void Interact(AActor* InteractableActor, const FLabInteractInputTemplate& InputTemplate);

private:
	bool bDetectionActive = false;
	float LastUpdateTime = 0.f;
	float PressDurationThreshold = 0.2f;
	bool bHoldProgressActive = false;
	FTimerHandle HoldDelayTimerHandle;
	FLabInteractableData TempInteractionData;

	TSubclassOf<UUserWidget> InitialWidgetClass;
	TArray<TSubclassOf<UUserWidget>> WidgetStack;
	
	UPROPERTY()
	TMap<ULabInteractInputKey*, float> InputStartTimes;

	UPROPERTY()
	TObjectPtr<AActor> NearbyInteractableActor;
	
	UPROPERTY(ReplicatedUsing = OnRep_bInteractionActive)
	bool bInteractionActive = false;

	UFUNCTION()
	void OnRep_bInteractionActive() const;

	UFUNCTION()
	void TraceInteractables(const float DeltaTime);
	
	UFUNCTION()
	AActor* PerformTrace() const;
	
	FORCEINLINE EDrawDebugTrace::Type GetDrawDebugType() const
	{
#if WITH_EDITOR
		return bShowDebugTraces ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
#else
		return EDrawDebugTrace::None;
#endif
	}
	
	UFUNCTION()
	AActor* FindNearestInteractable(const FVector& CurrentLocation, const TArray<FHitResult>& HitResults) const;

	UFUNCTION()
	void UpdateFocusedInteractable(AActor* InteractableActor);

	UFUNCTION()
	void UpdateWidgetContent(AActor* InteractableActor);
	
	UFUNCTION()
	void UpdateWidgetPlacement();
	
	UFUNCTION()
	void InitializeWidget();
	
	UFUNCTION()
	void BeginHoldProgress();

	UFUNCTION()
	void UpdateHoldInteraction(float DeltaTime);
};

