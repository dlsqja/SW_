// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"

#include "Main.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AExplosive::AExplosive()
{
	Sinsize=0;

	Damage=15.f;
}


void AExplosive::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	UE_LOG(LogTemp,Warning,TEXT("Explosivebegin"));

	if(OtherActor)//오버랩함수의 인자로, 충돌한 Actor임 
	{
		AMain* Main = Cast<AMain>(OtherActor);//있는지 확인하여 cast(형변환)한 뒤에, 함수를 실행시킴, AActor는 AMain의 부모이므로, 자식으로 형변환 해줘야함
		if(Main)
		{
			Main->DecrementHealth(Damage);

			if(OverlapParticleSystem)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),OverlapParticleSystem,GetActorLocation(),FRotator(0.f),true);
			}
			if(OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this,OverlapSound);
			}
			Destroy();
		}
	}
	
}


void AExplosive::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent ,AActor* OtherActor ,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AExplosive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Sinsize+=DeltaTime;
	float Floating = FMath::Sin(Sinsize);
	CollisionVolume->AddLocalOffset(FVector(0.f,0.f,Floating));
}