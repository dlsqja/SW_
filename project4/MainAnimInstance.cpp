// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"

#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpectatorPawn.h"

void UMainAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();//이 애니메이션 인스턴스의 소유Pawn을 자동으로 땡겨옴
		if(Pawn)
		{
			Main=Cast<AMain>(Pawn);
		}
	}
	
}

void UMainAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();//이 애니메이션 인스턴스의 소유Pawn을 자동으로 땡겨옴
	}

	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X,Speed.Y,0.f);
		MovementSpeed = LateralSpeed.Size();

		
		
		bIsInAir = Pawn->GetMovementComponent()->IsFalling();
		if(Main==nullptr)
		{
			Main=Cast<AMain>(Pawn);
		}
	}
	
}