﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "LabInteractionComponent.h"

#include "LabInteractableInterface.h"
#include "LabInteractInputKey.h"
#include "LabInteractionWidgetInterface.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LabInteraction/LabInteraction.h"
#include "Net/UnrealNetwork.h"

ULabInteractionComponent::ULabInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	TempInteractionData = FLabInteractableData();

	SetIsReplicatedByDefault(true);
	SetWidgetSpace(EWidgetSpace::Screen);
	SetDrawAtDesiredSize(true);
	SetPivot(FVector2d(0.f));
}

void ULabInteractionComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bInteractionActive);
}

void ULabInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bDetectionActive)
		return;
	
	TraceInteractables(DeltaTime);
	UpdateHoldInteraction(DeltaTime);
	UpdateWidgetPlacement();
}

void ULabInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	SetDetectionActive(true);

	InitializeWidget();
}

void ULabInteractionComponent::SetDetectionActive(const bool bNewActive)
{
	InitialWidgetClass = GetWidgetClass();
	if (!InitialWidgetClass)
	{
		UE_LOG(LogLabInteraction, Error, 
			TEXT("SetDetectionActive: InitialWidgetClass is null. Ensure that the widget class is properly set in the editor or code. Detection will be inactive."));
		bDetectionActive = false;
		return;
	}
	
	const APawn* Pawn = Cast<APawn>(GetOwner());
	if (!IsValid(Pawn))
	{
		UE_LOG(LogLabInteraction, Warning, TEXT("SetDetectionActive: Owner is not a valid Pawn. Detection will be inactive."));
		bDetectionActive = false;
		return;
	}

	// Check if the Pawn is locally controlled (important for multiplayer scenarios)
	if (!Pawn->IsLocallyControlled())
	{
		UE_LOG(LogLabInteraction, Warning, TEXT("SetDetectionActive: Pawn is not locally controlled. Detection will be inactive."));
		bDetectionActive = false;
		return;
	}

	const APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogLabInteraction, Warning, TEXT("SetDetectionActive: Pawn does not have a valid PlayerController. Detection will be inactive."));
		bDetectionActive = false;
		return;
	}
	
	bDetectionActive = bNewActive;
	
	UE_LOG(LogLabInteraction, Log, TEXT("SetDetectionActive: Detection active state updated. New state: %s | Role: %d"), 
		bDetectionActive ? TEXT("Active") : TEXT("Inactive"), GetOwner()->GetLocalRole());
}

void ULabInteractionComponent::SetInteractionActive(const bool bNewActive)
{
	if (bInteractionActive != bNewActive)
	{
		bInteractionActive = bNewActive;

		UE_LOG(LogLabInteraction, Log, TEXT("SetInteractionActive called. Owner: %s, New Active State: %s"), 
			*GetOwner()->GetName(), bInteractionActive ? TEXT("Active") : TEXT("Inactive"));
	
		OnRep_bInteractionActive();
	}
}

UUserWidget* ULabInteractionComponent::PushWidget(const TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (!NewWidgetClass)
	{
		UE_LOG(LogLabInteraction, Warning, TEXT("NewWidgetClass is null"));
		return nullptr;
	}
	
	const APawn* OwningActor = Cast<APawn>(GetOwner());
	if (OwningActor && OwningActor->IsLocallyControlled())
	{
		if (GetWidget()->IsA(NewWidgetClass))
		{
			return nullptr;
		}
		
		if (WidgetStack.Num() == 0 && InitialWidgetClass)
		{
			WidgetStack.Add(InitialWidgetClass);
		}

		if (!WidgetStack.Contains(NewWidgetClass))
		{
			WidgetStack.Add(NewWidgetClass);
		}
		
		SetWidgetClass(NewWidgetClass);
		InitializeWidget();

		UpdateWidgetContent(NearbyInteractableActor);
		return GetWidget();
	}

	UE_LOG(LogLabInteraction, Warning, TEXT("PushWidget: OwningActor is not locally controlled"));
	return nullptr;
}

void ULabInteractionComponent::PopWidget(UUserWidget*& OutActiveWidget)
{
	if (WidgetStack.Num() <= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("PopWidget: Cannot pop widget. Already at the initial widget."));
		OutActiveWidget = PushWidget(InitialWidgetClass);
		WidgetStack.Empty();
		return;
	}

	WidgetStack.Pop();
	OutActiveWidget = PushWidget(WidgetStack.Last());
}

void ULabInteractionComponent::InteractionInput(ULabInteractInputKey* InputKey, const bool bPressed)
{
	if (!IsValid(InputKey))
	{
		UE_LOG(LogLabInteraction, Error, TEXT("InteractionInput is called but InputKey is missing."))
		return;
	}

	if (!IsValid(NearbyInteractableActor))
	{
		UE_LOG(LogLabInteraction, Warning, TEXT("InteractionInput is called but no focused interactable actor."))
		return;
	}

	const float CurrentTimeSeconds = GetWorld()->GetTimeSeconds();

	if (bPressed)
	{
		if (InputStartTimes.Contains(InputKey))
		{
			UE_LOG(LogLabInteraction, Warning, TEXT("InteractionInput duplicate key press detected."));
			return;
		}
		
        // Record the time when the key is pressed
		InputStartTimes.Add(InputKey, CurrentTimeSeconds);

		bHoldProgressActive = false;
		GetWorld()->GetTimerManager().SetTimer(
			HoldDelayTimerHandle,
			this,
			&ThisClass::BeginHoldProgress,
			PressDurationThreshold,
			false);
	}
	else
	{
		// clear hold delay timer handle
		GetWorld()->GetTimerManager().ClearTimer(HoldDelayTimerHandle);
		
		const float* StartTime = InputStartTimes.Find(InputKey);
		if (!StartTime)
		{
			UE_LOG(LogLabInteraction, Warning, TEXT("InteractionInput released without matching press."));
			return;
		}

		// Calculate how long the input was held
		const float PressDuration = CurrentTimeSeconds - *StartTime;
		
		// Remove recorded key pressed
		InputStartTimes.Remove(InputKey);

		const TArray<FLabInteractInputTemplate>& InteractInputKeys = TempInteractionData.GetInputKeys();
		for (int Index = 0; Index < InteractInputKeys.Num(); ++Index)
		{
			if (InteractInputKeys[Index].InputKey.Get() != InputKey)
			{
				continue;
			}
			
			if (InteractInputKeys[Index].InteractionType == ELabInteractionType::InteractionType_Press)
			{
				if (PressDuration < PressDurationThreshold)
				{
					UE_LOG(LogLabInteraction, Log, TEXT("Short press detected."));
					Interact(NearbyInteractableActor, InteractInputKeys[Index]);
				}
				else
				{
					const bool ContainsHoldInput = InteractInputKeys.ContainsByPredicate([](const FLabInteractInputTemplate& Template)
					{
						return Template.InteractionType == ELabInteractionType::InteractionType_Hold;
					});

					if (!ContainsHoldInput)
					{
						UE_LOG(LogLabInteraction, Log, TEXT("Short press detected."));
						Interact(NearbyInteractableActor, InteractInputKeys[Index]);
					}
				}
			}
			else if (InteractInputKeys[Index].InteractionType == ELabInteractionType::InteractionType_Hold)
			{
				// Reset progress when key is released
				OnHoldProgressUpdated.Broadcast(InputKey, 0);
			
				if (PressDuration >= InteractInputKeys[Index].InteractionDuration)
				{
					UE_LOG(LogLabInteraction, Log, TEXT("Hold detected."));
					Interact(NearbyInteractableActor, InteractInputKeys[Index]);
				}
			}
		}
	}
}

void ULabInteractionComponent::Interact_Implementation(AActor* InteractableActor, const FLabInteractInputTemplate& InputTemplate)
{
	if (!IsValid(InteractableActor))
		return;

	if (!InputTemplate.InputKey.IsValid())
		return;

	ILabInteractableInterface::Execute_Interact(InteractableActor, InputTemplate.Name, this);
}

void ULabInteractionComponent::OnRep_bInteractionActive() const
{
	UUserWidget* InteractionWidget = GetWidget();
	if (!IsValid(InteractionWidget))
	{
		return;
	}

	if (!InteractionWidget->Implements<ULabInteractionWidgetInterface>())
	{
		return;
	}
	
	if (bInteractionActive && IsValid(NearbyInteractableActor))
	{
		InteractionWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ULabInteractionComponent::TraceInteractables(const float DeltaTime)
{
	LastUpdateTime += DeltaTime;
	if (LastUpdateTime >= DetectionFrequency)
	{
		LastUpdateTime = 0.f;

		UpdateFocusedInteractable(PerformTrace());
	}
}

AActor* ULabInteractionComponent::PerformTrace() const
{
	if (AActor* Owner = GetOwner())
	{
		static TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Reset();
		ActorsToIgnore.Add(Owner);
		
		const FVector WorldLocation = Owner->GetActorLocation();
		const ETraceTypeQuery TraceQuery = UEngineTypes::ConvertToTraceType(DetectionChannel);

		TArray<FHitResult> HitResults;
		UKismetSystemLibrary::SphereTraceMulti( GetWorld(), WorldLocation, WorldLocation, DetectionRadius, TraceQuery, false, ActorsToIgnore, GetDrawDebugType(), HitResults, true, FLinearColor::Red, FLinearColor::Green, DetectionFrequency);

		return FindNearestInteractable(WorldLocation, HitResults);
	}

	return nullptr;
}

AActor* ULabInteractionComponent::FindNearestInteractable(const FVector& CurrentLocation, const TArray<FHitResult>& HitResults) const
{
	if (HitResults.Num() == 0)
	{
		return nullptr;
	}

	float ClosestDistanceSquared = FLT_MAX;
	AActor* NearestInteractable = nullptr;

	for (const FHitResult& HitResult : HitResults)
	{
		if (AActor* HitActor = HitResult.GetActor())
		{
			if (HitActor->Implements<ULabInteractableInterface>())
			{
				const float DistanceSquared = FVector::DistSquared(CurrentLocation, HitActor->GetActorLocation());
				if (DistanceSquared < ClosestDistanceSquared)
				{
					ClosestDistanceSquared = DistanceSquared;
					NearestInteractable = HitActor;
				}
			}
		}
	}

	return NearestInteractable;
}

void ULabInteractionComponent::UpdateFocusedInteractable(AActor* InteractableActor)
{
	if (InteractableActor != NearbyInteractableActor)
	{
		if (NearbyInteractableActor)
		{
			ILabInteractableInterface::Execute_UpdateFocus(NearbyInteractableActor, false);
		}
		
		if (WidgetStack.Num() != 0)
		{
			WidgetStack.Empty();
			SetWidgetClass(InitialWidgetClass);
			InitializeWidget();
		}
		
		NearbyInteractableActor = InteractableActor;
		if (IsValid(NearbyInteractableActor))
		{
			ILabInteractableInterface::Execute_UpdateFocus(InteractableActor, true);

			UpdateWidgetContent(InteractableActor);
			SetInteractionActive(true);
		}
		else
		{
			TempInteractionData = FLabInteractableData();
			SetInteractionActive(false);
		}
	}
}

void ULabInteractionComponent::UpdateWidgetContent(AActor* InteractableActor)
{
	if (IsValid(InteractableActor) && GetWidget()->Implements<ULabInteractionWidgetInterface>())
	{
		ILabInteractableInterface::Execute_GetInteractableData(InteractableActor, TempInteractionData.Key, TempInteractionData);

		TArray<ULabInteractInputKeyInstance*> InputKeys;
		for (const FLabInteractInputTemplate InputTemplate : TempInteractionData.GetInputKeys())
		{
			ULabInteractInputKeyInstance* InputKey = NewObject<ULabInteractInputKeyInstance>();
			InputKey->DisplayText = InputTemplate.DisplayText;
			InputKey->InputKey = InputTemplate.InputKey.Get();
			InputKey->InteractionType = InputTemplate.InteractionType;
			InputKey->InteractionDuration = InputTemplate.InteractionDuration;
			InputKeys.Add(InputKey);
		}
		
		ILabInteractionWidgetInterface::Execute_UpdateWidgetContent(GetWidget(), this, TempInteractionData.DisplayText, InputKeys);
	}
}

void ULabInteractionComponent::UpdateWidgetPlacement()
{
	if (bInteractionActive && IsValid(NearbyInteractableActor))
	{
		FVector Origin;
        FVector BoxExtent;
		
		NearbyInteractableActor->GetActorBounds(false, Origin, BoxExtent);
		const FVector BoundingBoxCenter = Origin;
		SetWorldLocation(BoundingBoxCenter);
	}
}

void ULabInteractionComponent::InitializeWidget()
{
	InitWidget();

	UUserWidget* InteractionWidget = GetWidget();
	if (!IsValid(InteractionWidget))
	{
		return;
	}

	if (InteractionWidget->Implements<ULabInteractionWidgetInterface>())
	{
		ILabInteractionWidgetInterface::Execute_OnInitInteractionWidget(InteractionWidget, this);
	}

	InteractionWidget->SetVisibility(bInteractionActive ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}

void ULabInteractionComponent::BeginHoldProgress()
{
	if (!IsValid(NearbyInteractableActor))
		return;
	
	bHoldProgressActive = true;
}

void ULabInteractionComponent::UpdateHoldInteraction(float DeltaTime)
{
	if (!bHoldProgressActive)
		return;
	
	if (!IsValid(NearbyInteractableActor))
		return;
	
	if (InputStartTimes.IsEmpty())
		return;
	
	const TArray<FLabInteractInputTemplate>& InteractInputKeys = TempInteractionData.GetInputKeys();
	if (InteractInputKeys.IsEmpty())
		return;

	// Create a map for efficient lookup
	TMap<ULabInteractInputKey*, const FLabInteractInputTemplate*> InputKeyToTemplateMap;
	for (const FLabInteractInputTemplate& Template : InteractInputKeys)
	{
		InputKeyToTemplateMap.Add(Template.InputKey.Get(), &Template);
	}
	
	// Keys to remove after completion
	TArray<ULabInteractInputKey*> CompletedKeys;
	
	const float CurrentTimeSeconds = GetWorld()->GetTimeSeconds();
	for (const auto& Pair : InputStartTimes)
	{
		ULabInteractInputKey* InputKey = Pair.Key;
		const float StartTime = Pair.Value;
		const float ElapsedTime = CurrentTimeSeconds - StartTime;

		// Find the interaction template for this key
		const FLabInteractInputTemplate** InputTemplatePtr = InputKeyToTemplateMap.Find(InputKey);
		if (!InputTemplatePtr)
		{
			// Skip if no template is found
			continue;
		}
		
		const FLabInteractInputTemplate* InputTemplate = *InputTemplatePtr;
		if (InputTemplate->InteractionType == ELabInteractionType::InteractionType_Hold)
		{
			const float Progress = FMath::Clamp(ElapsedTime / InputTemplate->InteractionDuration, 0.0f, 1.0f);

			// Broadcast progress if it has changed significantly
			static constexpr float UpdateThreshold = 0.01f;
			static TMap<ULabInteractInputKey*, float> LastProgressMap;

			const float* LastProgress = LastProgressMap.Find(InputKey);
			if (!LastProgress || FMath::Abs(Progress - *LastProgress) >= UpdateThreshold)
			{
				OnHoldProgressUpdated.Broadcast(InputKey, Progress);
				LastProgressMap.Add(InputKey, Progress);
			}

			// Handle completion
			if (Progress >= 1.0f)
			{
				Interact(NearbyInteractableActor, *InputTemplate);
				bHoldProgressActive = false;
				
				UE_LOG(LogLabInteraction, Log, TEXT("Hold completed for InputKey."));
				OnHoldProgressUpdated.Broadcast(InputKey, 0);
				CompletedKeys.Add(InputKey);
				
				// Clean up progress tracking
				LastProgressMap.Remove(InputKey); 
			}
		}
	}
	
	// Remove completed keys after iteration
	for (const ULabInteractInputKey* CompletedKey : CompletedKeys)
	{
		InputStartTimes.Remove(CompletedKey);
	}
}
