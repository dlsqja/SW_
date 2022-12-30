// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Enemy.h"
#include "Main.h"
#include "Chaos/AABBTree.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AWeapon::AWeapon()
{
	SkeletalMesh=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletonMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	CombatCollision=CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetRootComponent());
	

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	CombatCollision->SetCollisionObjectType(ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
	
	
	bWeaponParticles=false;

	WeaponState = EWeaponState::EWS_Pickup;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	
	CombatCollision->OnComponentBeginOverlap.AddDynamic(this,&AWeapon::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this,&AWeapon::CombatOnOverlapEnd);
	
}
void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent,OtherActor,OtherComp,OtherBodyIndex,bFromSweep,SweepResult);
	if(WeaponState ==EWeaponState::EWS_Pickup && OtherActor)
	{
		UE_LOG(LogTemp,Warning,TEXT("overlap"));
		AMain* Main=Cast<AMain>(OtherActor);
		if(Main)
		{
			Main->SetActiveOverlappingItem(this);
		}
	}
}
void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent ,AActor* OtherActor ,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent,OtherActor,OtherComp,OtherBodyIndex);
	if(OtherActor)
	{
		UE_LOG(LogTemp,Warning,TEXT("overlapend"));
		AMain* Main=Cast<AMain>(OtherActor);
		if(Main)
		{
			Main->SetActiveOverlappingItem(nullptr);
		}
	}
}

void AWeapon::Equip(AMain* Char)
{
	if(Char)
	{
		SetInstigator(Char->GetController());
		
		CollisionVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera,ECollisionResponse::ECR_Ignore);
		CollisionVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Ignore);

		SkeletalMesh->SetSimulatePhysics(false);

		const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");
		if(RightHandSocket)
		{   Char->SetEquippedWeapon(this);
			RightHandSocket->AttachActor(this,Char->GetMesh());
			Char->SetActiveOverlappingItem(nullptr);
		}
		if(OnEquipSound) UGameplayStatics::PlaySound2D(this,OnEquipSound);
		if(!bWeaponParticles)
		{
			ParticleSystem->Deactivate();
		}
	}
}

void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if(OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if(Enemy)
		{
			if(Enemy->HitParticles)
			{
				const USkeletalMeshSocket* WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket");
				if(WeaponSocket)
				{
					FVector SocketLocation = WeaponSocket->GetSocketLocation(SkeletalMesh);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),Enemy->HitParticles,SocketLocation,FRotator(0.f),true);
				}
			}
			if(Enemy->HitSound)
			{
				UGameplayStatics::PlaySound2D(this,Enemy->HitSound);
			}
			if(DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Enemy,Damage,WeaponInstigator,this,DamageTypeClass);
			}
		}
	}
}
void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent ,AActor* OtherActor ,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void AWeapon::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}


void AWeapon::DeActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::PlaySwingSound()
{
	UGameplayStatics::PlaySound2D(this,SwingSound);
	
}