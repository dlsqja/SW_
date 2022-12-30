// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FirstSaveGame.h"
#include "ItemStorage.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

UENUM(BlueprintType)
enum class EMovementstatus : uint8
{
	EMS_normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DIsplayNmae = "Springting"),
	EMS_Dead UMETA(DIsplayNmae = "Dead"),

	EMS_Max UMETA(DIsplayNmae = "EMS_Max")
	
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DIsplayNmae = "BelowMinimum"),
	ESS_Exhausted UMETA(DIsplayNmae = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DIsplayNmae = "ExhaustedRecovering")
};


UCLASS()
class PROJECT4_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	TArray<FVector> PickupLocations;

	
	
	UFUNCTION(BlueprintCallable)
	void ShowPickupLocation();
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Enum")
	EMovementstatus Movementstatus;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Enum")
	EStaminaStatus StaminaStatus;

	FORCEINLINE void SetstaminaStatus(EStaminaStatus Status) {StaminaStatus=Status;}

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Movement")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Movement")
	float MinSprintStamina;
	
	//움직임 상태 설정
	void setMovementStatus(EMovementstatus status);

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Running")
	float RunningSpeed;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Running")
	float SprintingSpeed;

	UPROPERTY(BlueprintReadOnly,Category="Running")
	bool bShiftKeyDown;

	UPROPERTY(EditDefaultsOnly,Category="SaveData")
	TSubclassOf<AItemStorage> ItemStorage;

	
	/*
	 
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true")) //BP는 상속 개념이라 protect와 public만 접근 가능하지만 meta로  private도 접근 가능
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Camera,meta=(AllowPrivateAccess="true"))
	class UCameraComponent* FollowCamera;

	//카메라 회전의 스케일 값을 조정하는 변수
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Camera)
	float BaseTurAtRate;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Camera)
	float BaseLookupRate;

	/*UI에 업데이트 해줄 속성들
	 *
	 *
	 *
	*/

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Player Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Player Stats")
	float Health;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Player Stats")
	float MaxStamina;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Player Stats")
	float Stamina;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Player Stats")
	int32 Coins;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	
	void MoveForWard(float Value);

	void MoveRight(float Value);

	void Walk(void);

	void Run(void);

	void Zoom(float value);

	bool bLMBDown;
	void LMBDown();
	void LMBUp();
	void ESCDown();




	FORCEINLINE USpringArmComponent* GetCameraBoom() const {return CameraBoom;}
	FORCEINLINE UCameraComponent* GetFollowCamera() const {return FollowCamera;}

	void DecrementHealth(float value);

	void GetCoin(int32 value);

	void Die(void);

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Items")
	class AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Items")
	class AWeapon* ActiveOverlappingItem;

	void SetEquippedWeapon(AWeapon* WeaponToSet);
	FORCEINLINE AWeapon* GetEquippedWeapon(){return EquippedWeapon;}
	FORCEINLINE void SetActiveOverlappingItem(AWeapon* Item){ActiveOverlappingItem=Item;}

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Anim")
	bool bAttacking;

	void attack();

	UFUNCTION(BlueprintCallable,Category="Anim")
	void AttackEnd();

	void PunchAttack();
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Anim")
	class UAnimMontage* CombatMontage;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Particles")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Sound")
	class USoundBase* HitSound;

	float InterpSpeed;
	bool bInterpToEnemy;

	void SetInterpToEnemy(bool Interp);

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Combat")
	class AEnemy* CombatTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* Target) {CombatTarget=Target;}

	FRotator GetLookAtRotationYaw(FVector Target);

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(VisibleAnywhere,BlueprintReadonly,Category="Combat")
	bool bHasCombatTarget;

	FORCEINLINE void SetHasCombatTarget(bool HasTarget){bHasCombatTarget=HasTarget;}

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Controller")
	class AMainyPlayerController* MainyPlayerController;

	UPROPERTY(BlueprintReadWrite,VisibleAnywhere,Category="Combat")
	FVector CombatTargetLocation;

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	float Velocity;

	void UpdateCombatTarget();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Combat")
	TSubclassOf<AEnemy> EnemyFliter;

	void IncreaseHealth(float amount);

	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void LoadGame(bool SetPosition);

	UFUNCTION(BlueprintCallable)
	void LoadGameNoSwitch();

	void SwitchLevel(FName LevelName);

	
	
};
