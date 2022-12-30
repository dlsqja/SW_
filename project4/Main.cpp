// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"

#include <tuple>

#include "AIController.h"
#include "Enemy.h"
#include "MainyPlayerController.h"
#include "Weapon.h"
#include "Camera/CameraComponent.h"
#include "Chaos/ChaosDebugDraw.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bHasCombatTarget=false;
	
	CameraBoom=CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 400.f;//대상과의 거리
	CameraBoom->bUsePawnControlRotation = true;//컨트롤러에 의존하여 카메라 로테이팅을 허용

	FollowCamera=CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);//Socket은 붙을 위치를 말함   캐릭터로 따지면 손에 칼을 붙이는 것
	FollowCamera->bUsePawnControlRotation = false;//springarm과 동시에 입력받는걸 방지
	
	//최초 캡슐사이즈 정의
	GetCapsuleComponent()->SetCapsuleSize(34.f,100.f);

	//컨트롤러로 캐릭터 회전 금지
	bUseControllerRotationYaw=false;
	bUseControllerRotationPitch=false;
	bUseControllerRotationRoll=false;

	//움직임에 따라 캐릭터의 방향 회전을 자연스럽게 잡아줌 
	GetCharacterMovement()->bOrientRotationToMovement=true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f,400.f,0.0f);//회전율
	GetCharacterMovement()->JumpZVelocity = 450.f;
	GetCharacterMovement()->AirControl = 0.8f;

	
	
	MaxHealth=100.f;
	
	Health=100.f;

	MaxStamina=350.f;

	Stamina=350.f;

	Coins=0;

	RunningSpeed=400.f;

	SprintingSpeed=700.f;

	bShiftKeyDown=false;

	//초기화
	Movementstatus=EMovementstatus::EMS_normal;
	StaminaStatus=EStaminaStatus::ESS_normal;

	StaminaDrainRate = 25.f;
	MinSprintStamina = 150.f;

	bLMBDown=false;

	bAttacking=false;

	InterpSpeed=15.f;
	bInterpToEnemy=false;

	Velocity=0.f;
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();
	
	//UKismetSystemLibrary::DrawDebugSphere(this,GetActorLocation()+FVector(0,0,125.f),25.f,8,FLinearColor::Green,5.f,1.f);

	MainyPlayerController=Cast<AMainyPlayerController>(GetController());

	
	if(FName(GetWorld()->GetMapName())==FName("UEDPIE_0_ElvenRuins"))
	{
		UE_LOG(LogTemp,Warning,TEXT("%s"),*FName(GetWorld()->GetMapName()).ToString());
		LoadGameNoSwitch();
	}
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Velocity=GetVelocity().Size();
	
	if(Movementstatus==EMovementstatus::EMS_Dead) return;

	if(bInterpToEnemy&&CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(),LookAtYaw,DeltaTime,InterpSpeed);

		SetActorRotation(InterpRotation); 
	}
	if(CombatTarget)
	{
		CombatTargetLocation=CombatTarget->GetActorLocation();
		if(MainyPlayerController)
		{
			MainyPlayerController->EnemyLocation=CombatTargetLocation;
		}
	}

	
	float DeltaStamina = StaminaDrainRate * DeltaTime;

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_normal:

		if(bShiftKeyDown&&Velocity!=0)
		{
			if(Stamina-DeltaStamina<=MinSprintStamina)
			{
				SetstaminaStatus(EStaminaStatus::ESS_BelowMinimum);
				Stamina -=DeltaStamina;
			}
			else
			{
				Stamina -=DeltaStamina;
			}
			setMovementStatus(EMovementstatus::EMS_Sprinting);
		}
		else//쉬프트를 안누를 때,
			{
			if(Stamina + DeltaStamina >=MaxStamina)
			{
				Stamina = MaxStamina;
			}
			else
			{
				Stamina +=DeltaStamina;	
			}
			setMovementStatus(EMovementstatus::EMS_normal);
			}
		break;
	case EStaminaStatus::ESS_BelowMinimum:
		if(bShiftKeyDown&&Velocity!=0)
		{
			if(Stamina-DeltaStamina<=0.f)
			{
				SetstaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina =0.f;
				setMovementStatus(EMovementstatus::EMS_normal);
			}
			else
			{
				Stamina -=DeltaStamina;
				setMovementStatus(EMovementstatus::EMS_Sprinting);
			}
		}
		else
		{
			if(Stamina+DeltaStamina>=MinSprintStamina)
			{
				SetstaminaStatus(EStaminaStatus::ESS_normal);
				Stamina +=DeltaStamina;
			}
			else
			{
				Stamina +=DeltaStamina;
			}
			setMovementStatus(EMovementstatus::EMS_normal);
		}

		break;
	case EStaminaStatus::ESS_Exhausted:
		if(bShiftKeyDown&&Velocity!=0)
		{
			Stamina=0.f;
		}
		else
		{
			{
				SetstaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
				Stamina+=DeltaStamina;
			}
		}
		setMovementStatus(EMovementstatus::EMS_normal);
		break;
	case EStaminaStatus::ESS_ExhaustedRecovering:
		if(Stamina + DeltaStamina>=MinSprintStamina)
		{
			SetstaminaStatus(EStaminaStatus::ESS_normal);
			Stamina +=DeltaStamina;
		}
		else
		{
			Stamina += DeltaStamina;
		}
		setMovementStatus(EMovementstatus::EMS_normal);

		break;
	default:
		;
	}
	

}

FRotator AMain::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),Target);
	FRotator LookAtRotationYaw(0.f,LookAtRotation.Yaw,0.f);

	return LookAtRotationYaw;
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump",IE_Released,this,&ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Run",IE_Pressed,this,&AMain::Run);
	PlayerInputComponent->BindAction("Run",IE_Released,this,&AMain::Walk);

	PlayerInputComponent->BindAction("LMB",IE_Pressed,this,&AMain::LMBDown);
	PlayerInputComponent->BindAction("LMB",IE_Released,this,&AMain::LMBUp);

	PlayerInputComponent->BindAction("ESC",IE_Pressed,this,&AMain::ESCDown);
	
	PlayerInputComponent->BindAxis("MoveForward",this,&AMain::MoveForWard);
	PlayerInputComponent->BindAxis("MoveRight",this,&AMain::MoveRight);

	PlayerInputComponent->BindAxis("MoveForward",this,&AMain::MoveForWard);
	PlayerInputComponent->BindAxis("MoveRight",this,&AMain::MoveRight);
	
	PlayerInputComponent->BindAxis("CameraYaw",this,&APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("CameraPitch",this,&APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("Zoom",this,&AMain::Zoom);
}

void AMain::Run()
{
	bShiftKeyDown=true;
}

void AMain::Walk()
{
	bShiftKeyDown=false;
}

void AMain::MoveForWard(float Value)
{
	if( Controller != nullptr && Value !=0.0f&&(!bAttacking))
	{
		// 어디가 앞인지 찾는 과정
		const FRotator Rotation = Controller -> GetControlRotation();
		const FRotator YawRotation(0.f,Rotation.Yaw,0.f);

		
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction,Value);
		
	}
}

void AMain::MoveRight(float Value)
{
	if( Controller != nullptr && Value !=0.0f&&(!bAttacking))
	{
		// 어디가 앞인지 찾는 과정
		const FRotator Rotation = Controller -> GetControlRotation();
		const FRotator YawRotation(0.f,Rotation.Yaw,0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction,Value);
		
	}
	
}

void AMain::Zoom(float Rate)
{
	FVector now=CameraBoom->GetRelativeLocation();
	now.Z=FMath::Clamp(now.Z+-5*Rate,50.f,70.f);
	CameraBoom->SetRelativeLocation(now);
	CameraBoom->TargetArmLength=FMath::Clamp(CameraBoom->TargetArmLength+30*Rate,200.f,600.f);  
}

void AMain::DecrementHealth(float value)
{
	Health-=value;
	if(Health<=0.f&&Movementstatus!=EMovementstatus::EMS_Dead)
	{
		Die();
	}
}

void AMain::Die()
{
	APlayerController* DisableController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	DisableInput(DisableController);
	UAnimInstance* Anim = GetMesh()->GetAnimInstance();
	setMovementStatus(EMovementstatus::EMS_Dead);
	if (Anim)
	{
		Anim->Montage_Play(CombatMontage, 1.4f);
		Anim->Montage_JumpToSection(FName("Death"), CombatMontage);
	}
}

void AMain::DeathEnd()
{
	GetMesh()->bPauseAnims=true;
	GetMesh()->bNoSkeletonUpdate=true;
}

void AMain::GetCoin(int32 value)
{
	Coins+=value;
}

void AMain::setMovementStatus(EMovementstatus status)
{
	Movementstatus = status;
	if(Movementstatus == EMovementstatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
		
	}
}
void AMain::ShowPickupLocation()
{
	for(auto FVector: PickupLocations)
	{
		UKismetSystemLibrary::DrawDebugSphere(this,FVector,25.f,8,FLinearColor::Green,5.f,1.f);
	}
}

void AMain::LMBDown()
{
	bLMBDown =true;

	if(ActiveOverlappingItem)
	{
		ActiveOverlappingItem->Equip(this);
	}
	else if(EquippedWeapon)
	{
		attack();
	}
	else
	{
		PunchAttack();
	}
}
void AMain::LMBUp()
{
	bLMBDown =false;
}

void AMain::ESCDown()
{
	if(MainyPlayerController)
	{
		MainyPlayerController->PauseMenuToggle();
	}
}

void AMain::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	if(EquippedWeapon!=nullptr)
	{
		EquippedWeapon->Destroy();
	}
	EquippedWeapon = WeaponToSet;
}

void AMain::attack()
{
	if(!bAttacking)
	{
		bAttacking=true;
		SetInterpToEnemy(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(EquippedWeapon)
		{
			if (AnimInstance && CombatMontage)
			{
				int32 Section = FMath::RandRange(0, 1);
				switch (Section)
				{
				case 0:
					AnimInstance->Montage_Play(CombatMontage, 2.2f);
					AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
					break;

				case 1:
					AnimInstance->Montage_Play(CombatMontage, 1.8f);
					AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);

					break;
				default:
					;
				}
			}
		}
	}
}


void AMain::PunchAttack()
{
	if(!bAttacking)
	{
		UE_LOG(LogTemp,Warning,TEXT("Punch"));
		bAttacking=true;
	
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			AnimInstance->Montage_Play(CombatMontage, 1.5f);
			AnimInstance->Montage_JumpToSection(FName("Punch"), CombatMontage);
		}
	}
}

void AMain::AttackEnd()
{
	UE_LOG(LogTemp,Warning,TEXT("attackend"));
	bAttacking=false;
	SetInterpToEnemy(false);
	if(bLMBDown)
	{
		attack();
	}
}

void AMain::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy=Interp;
}

float AMain::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Health-=DamageAmount;
	if(Health<=0.f&&Movementstatus!=EMovementstatus::EMS_Dead)
	{
		Die();
		//주인공이 죽은 후에 공격하는 것을 멈춤
		if(DamageCauser)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if(Enemy)
			{
				Enemy->bHasVaildTarget=false;
			}
		}
	}
	return DamageAmount;
}

void AMain::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors,EnemyFliter);

	if(OverlappingActors.Num() == 0) return;

	UE_LOG(LogTemp,Warning,TEXT("Update!"));
	AEnemy* CloestEnemy = Cast<AEnemy>(OverlappingActors[0]);
	if(CloestEnemy)
	{
		float MinDistance = (CloestEnemy->GetActorLocation()-GetActorLocation()).Size();

		for(auto Actor : OverlappingActors)
		{
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if(Enemy)
			{
				float DistanceToActor = (Enemy->GetActorLocation()-GetActorLocation()).Size();
				if(DistanceToActor<MinDistance)
				{
					MinDistance = DistanceToActor;
					CloestEnemy = Enemy;
				}
			}
		}
		if(MainyPlayerController)
		{
			MainyPlayerController->DisplayEnemyHealthBar();
		}
		SetCombatTarget(CloestEnemy);
		bHasCombatTarget=true;
	}
	
		

}
void AMain::IncreaseHealth(float amount)
{
	Health+=amount;
	if(Health>MaxHealth) Health=MaxHealth;
}

void AMain::SaveGame()
{
	UFirstSaveGame* SaveGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	SaveGameInstance->CharacterStats.Health=Health;
	SaveGameInstance->CharacterStats.MaxHealth=MaxHealth;
	SaveGameInstance->CharacterStats.Coins=Coins;
	SaveGameInstance->CharacterStats.MaxStamina=MaxStamina;
	SaveGameInstance->CharacterStats.Stamina=Stamina;
	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	SaveGameInstance->CharacterStats.LevelName=MapName;
	

	SaveGameInstance->CharacterStats.Location=GetActorLocation();
	SaveGameInstance->CharacterStats.Rotation=GetActorRotation();

	if(EquippedWeapon)
	{
		SaveGameInstance->CharacterStats.WeaponName=EquippedWeapon->Name;
	}
	

	UGameplayStatics::SaveGameToSlot(SaveGameInstance,SaveGameInstance->PlayerName,SaveGameInstance->UserIndex);
}

void AMain::LoadGame(bool SetPosition)
{
	UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName,LoadGameInstance->UserIndex));
	Health=LoadGameInstance->CharacterStats.Health;
	MaxHealth=LoadGameInstance->CharacterStats.MaxHealth;
	Coins=LoadGameInstance->CharacterStats.Coins;
	MaxStamina=LoadGameInstance->CharacterStats.MaxStamina;
	Stamina=LoadGameInstance->CharacterStats.Stamina;

	if(ItemStorage)
	{
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(ItemStorage);
		if(Weapons)
		{
			if(LoadGameInstance->CharacterStats.WeaponName!=TEXT(""))
			{
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[LoadGameInstance->CharacterStats.WeaponName]);
				if(WeaponToEquip) WeaponToEquip->Equip(this);
			}
			
		}
	}
	
	if(SetPosition)
	{
		SetActorLocation(LoadGameInstance->CharacterStats.Location);
		SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
	}
	if(LoadGameInstance->CharacterStats.LevelName!="")
	{
		FName LevelName(LoadGameInstance->CharacterStats.LevelName);

		SwitchLevel(LevelName);
	}
}

void AMain::LoadGameNoSwitch()
{
	UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName,LoadGameInstance->UserIndex));
	Health=LoadGameInstance->CharacterStats.Health;
	MaxHealth=LoadGameInstance->CharacterStats.MaxHealth;
	Coins=LoadGameInstance->CharacterStats.Coins;
	MaxStamina=LoadGameInstance->CharacterStats.MaxStamina;
	Stamina=LoadGameInstance->CharacterStats.Stamina;

	if(ItemStorage)
	{
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(ItemStorage);
		if(Weapons)
		{
			if(LoadGameInstance->CharacterStats.WeaponName!="")
			{
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[LoadGameInstance->CharacterStats.WeaponName]);
				if(WeaponToEquip) WeaponToEquip->Equip(this);
			}
			
		}
	}
}

void AMain::SwitchLevel(FName LevelName)
{
	UWorld* World=GetWorld();
	if(World)
	{
		FString CurrentLevel = World->GetMapName();

		FName CurrentLevelName(CurrentLevel);
		if(CurrentLevelName != LevelName)
		{
			UGameplayStatics::OpenLevel(World,LevelName);
		}
	}
	FInputModeGameOnly GameMode;
	MainyPlayerController->SetInputMode(GameMode);
}
