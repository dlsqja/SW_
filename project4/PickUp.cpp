// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUp.h"

#include "Main.h"
#include "Kismet/GameplayStatics.h"

APickUp::APickUp()
{
	CoinCount=2;
}



void APickUp::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	UE_LOG(LogTemp,Warning,TEXT("Pickupbegin"));
}

void APickUp::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent ,AActor* OtherActor ,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	
	UE_LOG(LogTemp,Warning,TEXT("Pickupend"));
	if(OtherActor)
	{
		AMain* Main=Cast<AMain>(OtherActor);
		if(Main)
		{
			if(OverlapParticleSystem)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),OverlapParticleSystem,GetActorLocation(),FRotator(0.f),true);
			}
			if(OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this,OverlapSound);
			}
			Main->GetCoin(CoinCount);
			Main->PickupLocations.Add(GetActorLocation());

			Destroy();
		}
	}
}

void APickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	StaticMesh->AddRelativeRotation(FRotator(0.f,3.f,0.f));
}