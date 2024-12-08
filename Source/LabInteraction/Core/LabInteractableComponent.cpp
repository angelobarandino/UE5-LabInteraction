// Fill out your copyright notice in the Description page of Project Settings.


#include "LabInteractableComponent.h"

#include "Net/UnrealNetwork.h"


ULabInteractableComponent::ULabInteractableComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	
	SetIsReplicatedByDefault(true);
	SetWidgetSpace(EWidgetSpace::Screen);
	SetDrawAtDesiredSize(true);
	SetPivot(FVector2d(0.f));
}

void ULabInteractableComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bInteractionActive);
}

void ULabInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void ULabInteractableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULabInteractableComponent::OnReplicated_bInteractionActive()
{
}
