// Fill out your copyright notice in the Description page of Project Settings.


#include "ColliderMovementComponent.h"

void UColliderMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime,TickType,ThisTickFunction);

	if(!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	FVector DesiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(5.0f);

	if(!DesiredMovementThisFrame.IsNearlyZero())
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(DesiredMovementThisFrame,UpdatedComponent->GetComponentRotation(),true,Hit);
		
		// 만약 hit이 발생 했을 때,
		if(Hit.IsValidBlockingHit())
		{
			SlideAlongSurface(DesiredMovementThisFrame,1.f-Hit.Time,Hit.Normal,Hit);
			UE_LOG(LogTemp,Warning,TEXT("HIT"));
		}
	}
}