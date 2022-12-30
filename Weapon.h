// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EWeaponState :uint8
{
	EWS_Pickup UMETA(DisplayName="Pickup"),
	EWS_Equipped UMETA(DisplayName="Equipped"),

	EWS_MAX UMETA(DisplayName="DefaultMax")
};



UCLASS()
class PROJECT4_API AWeapon : public AItem
{
	GENERATED_BODY()

public:

	AWeapon();

	EWeaponState WeaponState;

	UPROPERTY(EditDefaultsOnly,Category="SaveData")
	FString Name;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Item | Particles")
	bool bWeaponParticles;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Item | Sound")
	class USoundBase* OnEquipSound;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Item | Sound")
	class USoundBase* SwingSound;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="SkeletalMesh")
	class USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Item | Combat")
	class UBoxComponent* CombatCollision;
	
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent ,AActor* OtherActor ,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION()
	virtual void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent ,AActor* OtherActor ,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void Equip(class AMain* Char);

	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly,BlueprintReadWrite,Category="Item | Combat")
	float Damage;

	UFUNCTION(BlueprintCallable)
	void ActivateCollision();

	UFUNCTION(BlueprintCallable)
	void DeActivateCollision();

	
	FORCEINLINE void SetweaponState(EWeaponState State) {WeaponState=State;}
	FORCEINLINE EWeaponState GetWeaponState() {return WeaponState;}

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Combat")
	AController* WeaponInstigator;

	FORCEINLINE void SetInstigator(AController* Inst){WeaponInstigator=Inst;}
};
