// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include <tuple>

#include "AIController.h"
#include "Main.h"
#include "MainyPlayerController.h"
#include "PickUp.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystem.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere=CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.f);

	CombatSphere=CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);

	HitParticles=CreateDefaultSubobject<UParticleSystem>(TEXT("HitParticles"));

	EnemyStatement=EEnemyMovementStatus::EMS_Idle;
	bOverlappingCombatSphere=false;

	CombatCollision= CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetMesh(),FName("EnemySocket"));

	EnemyStatement=EEnemyMovementStatus::EMS_Idle;

	Health=75.f;
	MaxHealth=100.f;
	Damage=10.f;

	bAttacking=false;

	DeathDelay=4.f;

	bHasVaildTarget=false;
	
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AIController=Cast<AAIController>(GetController());
	
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this,&AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this,&AEnemy::AgroSphereOnOverlapEnd);
	CombatSphere->OnComponentBeginOverlap.AddDynamic(this,&AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this,&AEnemy::CombatSphereOnOverlapEnd);
	CombatCollision->OnComponentBeginOverlap.AddDynamic(this,&AEnemy::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this,&AEnemy::CombatOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	CombatCollision->SetCollisionObjectType(ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);

	GetMesh()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	
	AMain* Main = Cast<AMain>(OtherActor);
	if(Main)
	{
		bHasVaildTarget=true;
		Main->SetHasCombatTarget(true);
		Main->SetCombatTarget(this);
		CombatTarget=Main;
		if(Main->MainyPlayerController)
		{
			Main->MainyPlayerController->DisplayEnemyHealthBar();
		}
		MoveToTarget(Main);
	}
}
void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent ,AActor* OtherActor ,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor)
	{
		
		AMain* Main=Cast<AMain>(OtherActor);
		
		if(Main)
		{
			bHasVaildTarget=false;
			Main->SetHasCombatTarget(false);
			Main->SetCombatTarget(nullptr);
			if(Main->MainyPlayerController)
			{
				Main->MainyPlayerController->RemoveEnemyHealthBar();
			}
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			if(AIController)
			{
				AIController->StopMovement();
			}
		}
		
	}
}
void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if(OtherActor&&Alive())
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if(Main)
		{
			
			bOverlappingCombatSphere=true;
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
			GetWorldTimerManager().SetTimer(MoveHandler,this,&AEnemy::attack,0.5);
			
		}
		
	}
}
void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent ,AActor* OtherActor ,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor&&OtherComp)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		
		if(Main)
		{
			Main->UpdateCombatTarget();
			bOverlappingCombatSphere=false;
		}
		
		
		
	}
}

void AEnemy::MoveToTarget(AMain* Target)
{
	if(Alive())
	{
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
	
		if(AIController)
		{
		
			FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalActor(Target);
			MoveRequest.SetAcceptanceRadius(5.f); //얼만큼 가까워져야 성공으로 할것인가?
		
			FNavPathSharedPtr NavPath;

			AIController->MoveTo(MoveRequest, &NavPath);

			//이동경로 debugSphere로 표시
			// if(NavPath)
			// {
			// 	auto PathPoints = NavPath->GetPathPoints();
			//
			// 	for (auto Point : PathPoints)
			// 	{
			// 		FVector Location = Point.Location;
			//
			// 		UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Green, 10.f, 1.5f);
			// 	}
			// }
		
		}
	}
	
}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if(OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if(Main)
		{
			if(Main->HitParticles)
			{
				const USkeletalMeshSocket* HitSocket = GetMesh()->GetSocketByName("HitSocket");
				if(HitSocket)
				{
					FVector SocketLocation = HitSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),Main->HitParticles,SocketLocation,FRotator(0.f),true);
				}
			}
			if(Main->HitSound)
			{
				UGameplayStatics::PlaySound2D(this,Main->HitSound);
			}
			if(DamageTypeClass)//메인의 TakeDamage 함수 호출
				{
				UE_LOG(LogTemp,Warning,TEXT("Attack"));
				UGameplayStatics::ApplyDamage(Main,Damage,AIController,this,DamageTypeClass);
				}
		}
	}
}
void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent ,AActor* OtherActor ,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void AEnemy::ActivateCollision() const
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeActivateCollision() const
{ 
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
void AEnemy::PlaySwingSound() const
{
	if(SwingSound)
	{
		UGameplayStatics::PlaySound2D(this,SwingSound);
	}
}

void AEnemy::attack()
{
	if (Alive()&&bHasVaildTarget)
	{
		bAttacking = true;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			int RandomSwitch = FMath::RandRange(0, 2);

			switch (RandomSwitch)
			{
			case 0:

				AnimInstance->Montage_Play(CombatMontage, 1.5f);
				AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
				break;

			case 1:

				AnimInstance->Montage_Play(CombatMontage, 1.5f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				break;

			case 2:

				AnimInstance->Montage_Play(CombatMontage, 1.5f);
				AnimInstance->Montage_JumpToSection(FName("Attack_3"), CombatMontage);
				break;

			default:

				break;
			}
		}
	}
}

void AEnemy::attackend()
{
	UE_LOG(LogTemp,Warning,TEXT("attackend"));
	bAttacking = false;
	if(Alive())
	{
		float AttackTime= FMath::FRandRange(AttackMinTime,AttackMaxTime);
		GetWorldTimerManager().SetTimer(AttackTimer,this,&AEnemy::attack,AttackTime);
	}
	
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp,Warning,TEXT("attack"));
	Health-=DamageAmount;
	if(Health<=0.f)
	{
		Die(DamageCauser);
	}
	return DamageAmount;
}

void AEnemy::Die(AActor* Causer)
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(CombatMontage, 1.0f);
	AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);

	AMain* Main = Cast<AMain>(Causer);
	if(Main)
	{
		Main->UpdateCombatTarget();
	}
	
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);

	
}

void AEnemy::DeathEnd()
{
	GetMesh()->bPauseAnims=true;
	GetMesh()->bNoSkeletonUpdate=true;

	GetWorldTimerManager().SetTimer(Handler,this,&AEnemy::Disappar,DeathDelay);
}

bool AEnemy::Alive()
{
	return GetEnemyMovementStatus() !=EEnemyMovementStatus::EMS_Dead;
}

void AEnemy::Disappar()
{
	const USkeletalMeshSocket* DeathSocket = GetMesh()->GetSocketByName("DeathSocket");
	if(DeathSocket)
	{
		FVector SocketLocation = DeathSocket->GetSocketLocation(GetMesh());
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),DeathParticles,SocketLocation,FRotator(0.f),true);
	}
	
	FActorSpawnParameters SpawnParams;
	AActor* Actor =GetWorld()->SpawnActor<APickUp>(Coins,GetActorLocation(),FRotator(0.f),SpawnParams);
	
	Destroy();
}

