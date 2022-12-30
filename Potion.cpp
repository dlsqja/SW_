// Fill out your copyright notice in the Description page of Project Settings.


#include "Potion.h"

#include <dsound.h>

#include "Main.h"
#include "Kismet/GameplayStatics.h"

APotion::APotion()
{
	HealAmount=10;
}


void APotion::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	AMain* Main = Cast<AMain>(OtherActor);
	if(Main)
	{
		Main->IncreaseHealth(HealAmount);
		if(OverlapParticleSystem)
		{
			UGameplayStatics::SpawnEmitterAtLocation(this,OverlapParticleSystem,GetActorLocation());
		}
		if(OverlapSound)
		{
			UGameplayStatics::PlaySound2D(this,OverlapSound);
		}
		Destroy();
	}
	
	
}
void APotion::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent ,AActor* OtherActor ,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);	
}