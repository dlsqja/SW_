 // Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"

// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent=Mesh;

	StartPoint=FVector(0.f);
	EndPoint=FVector(0.f);


	bInterping=false;
	
	Interspeed=2.0f;  
	Intertime=3.f;
	
}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	StartPoint=GetActorLocation();
	EndPoint += StartPoint;

	bInterping = false;

	GetWorldTimerManager().SetTimer(InterpTimer,this,&AFloatingPlatform::ToggleInterpint,Intertime);

	Distance = (EndPoint - StartPoint).Size();
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bInterping)
	{
		FVector CurrentLocation = GetActorLocation();

		FVector Interp =  FMath::VInterpTo(CurrentLocation,EndPoint,DeltaTime,Interspeed);

		SetActorLocation(Interp);

		float DistanceTravled =  (GetActorLocation()-StartPoint).Size();
		if(Distance - DistanceTravled<=10.f)
		{
			ToggleInterpint();

			SwapVectors(StartPoint,EndPoint);
			GetWorldTimerManager().SetTimer(InterpTimer,this,&AFloatingPlatform::ToggleInterpint,Intertime);
			
		}
	}
	
}

void AFloatingPlatform::ToggleInterpint()
{
	bInterping = !bInterping;
}

void  AFloatingPlatform::SwapVectors(FVector& VectorOne,FVector& VectorTwo)
{
	FVector Temp = VectorOne;
	VectorOne = VectorTwo;
	VectorTwo=Temp;
}
