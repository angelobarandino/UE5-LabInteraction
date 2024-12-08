// Fill out your copyright notice in the Description page of Project Settings.


#include "LabInteractionComponent.h"

#include "LabInteractableComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LabInteraction/LabInteraction.h"
#include "Net/UnrealNetwork.h"

ULabInteractionComponent::ULabInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	SetVisibility(false);
	SetIsReplicatedByDefault(true);
	SetWidgetSpace(EWidgetSpace::Screen);
	SetDrawAtDesiredSize(true);
	SetPivot(FVector2d(0.f));
}

void ULabInteractionComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bInteractionActive);
	DOREPLIFETIME(ThisClass, FocusedInteractableActor);
}

void ULabInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bDetectionActive)
		return;

	UpdateTraceInteractable(DeltaTime);
}

void ULabInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	SetDetectionActive(true);
}

void ULabInteractionComponent::SetDetectionActive(const bool bNewActive)
{
	const APawn* Pawn = Cast<APawn>(GetOwner());
	if (!IsValid(Pawn))
	{
		UE_LOG(LogLabInteraction, Warning, TEXT("[ULabInteractComponent] SetDetectionActive: Owner is not a valid Pawn. Detection will be inactive."));
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
}

void ULabInteractionComponent::UpdateTraceInteractable(const float DeltaTime)
{
	LastUpdateTime += DeltaTime;
	if (LastUpdateTime >= DetectionFrequency)
	{
		LastUpdateTime = 0.f;

		UpdateFocusedInteractable(PerformTrace());
	}
}

EDrawDebugTrace::Type ULabInteractionComponent::GetDrawDebugType() const
{
#if WITH_EDITOR
	return bShowDebugTraces ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
#else
	return EDrawDebugTrace::None;
#endif
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
	if (InteractableActor != FocusedInteractableActor)
	{
		if (FocusedInteractableActor)
		{
			ILabInteractableInterface::Execute_UpdateFocus(FocusedInteractableActor, false);
		}
    
		if (IsValid(InteractableActor))
		{
			ILabInteractableInterface::Execute_UpdateFocus(InteractableActor, true);
			OnRep_FocusedInteractableActor();
			SetInteractionActive(true);
		}
		else
		{
			SetInteractionActive(false);
		}
		
		FocusedInteractableActor = InteractableActor;
	}
}

void ULabInteractionComponent::SetInteractionActive(const bool bNewActive)
{
	if (GetOwner()->GetLocalRole() < ROLE_Authority)
	{
		UE_LOG(LogLabInteraction, Warning, TEXT("SetInteractionActive called on client! Role: %d"), GetOwner()->GetLocalRole());
		return;
	}
	
	UE_LOG(LogLabInteraction, Log, TEXT("SetInteractionActive called. Owner: %s, New Active State: %s"), 
		*GetOwner()->GetName(), bNewActive ? TEXT("Active") : TEXT("Inactive"));

	bInteractionActive = bNewActive;

	UE_LOG(LogLabInteraction, Log, TEXT("Interaction Active State updated. New state: %s"), 
		bInteractionActive ? TEXT("Active") : TEXT("Inactive"));
	
	OnRep_bInteractionActive();
}

void ULabInteractionComponent::OnRep_bInteractionActive()
{
	UUserWidget* InteractionWidget = GetWidget();
	if (!IsValid(InteractionWidget))
	{
		return;
	}
	
	if (bInteractionActive)
	{
		SetVisibility(true);
		InteractionWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		SetVisibility(false);
		InteractionWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ULabInteractionComponent::OnRep_FocusedInteractableActor()
{
	if (IsValid(FocusedInteractableActor))
	{
		const FVector InteractableLocation = FocusedInteractableActor->GetActorLocation();
		SetWorldLocation(InteractableLocation);
		SetUsingAbsoluteLocation(true);
	}
}
