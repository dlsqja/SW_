// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUp.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"
UENUM(BlueprintType)
enum class EEnemyMovementStatus :uint8
{
	EMS_Idle  UMETA(DisplayNmae="Idle"),
	EMS_MoveToTarget  UMETA(DisplayNmae="MoveToTarget"),
	EMS_Attacking  UMETA(DisplayNmae="Attacking"),
	EMS_Dead  UMETA(DisplayNmae="Dead"),

	EMS_MAX  UMETA(DisplayNmae="DefaultMAX"),
};


UCLASS()
class PROJECT4_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Combat")
	class UBoxComponent* CombatCollision;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Movement")
	EEnemyMovementStatus EnemyStatement;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AI")
	USphereComponent* AgroSphere;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AI")
	USphereComponent* CombatSphere;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AI")
	class AAIController* AIController;

	

	

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus status){EnemyStatement = status;}

	UFUNCTION(BlueprintPure)
	FORCEINLINE EEnemyMovementStatus GetEnemyMovementStatus(){return EnemyStatement;}
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	virtual void AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent ,AActor* OtherActor ,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent ,AActor* OtherActor ,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent ,AActor* OtherActor ,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(class AMain* Target);

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="AI")
	bool bOverlappingCombatSphere;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="AI")
	AMain* CombatTarget;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AI")
	float Health;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AI")
	float MaxHealth;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AI")
	float Damage;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AI")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AI")
	class UParticleSystem* DeathParticles;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AI")
	class USoundBase* HitSound;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Item | Sound")
	class USoundBase* SwingSound;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Anim")
	class UAnimMontage* CombatMontage;

	UFUNCTION(BlueprintCallable)
	void ActivateCollision() const;

	UFUNCTION(BlueprintCallable)
	void DeActivateCollision() const;

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound() const;

	UFUNCTION(BlueprintCallable)
	void attack();

	UFUNCTION(BlueprintCallable)
	void attackend();

	bool bAttacking;

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="AttackTime")
	float AttackMinTime;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="AttackTime")
	float AttackMaxTime;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Coins")
    TSubclassOf<APickUp> Coins;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void Die(AActor* Causer);

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	bool Alive();

	void Disappar();

	FTimerHandle Handler;

	float DeathDelay;

	bool bHasVaildTarget;

	FTimerHandle MoveHandler;

	
};
