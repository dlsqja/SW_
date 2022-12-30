// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorSwitch.generated.h"

UCLASS()
class PROJECT4_API AFloorSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloorSwitch();
	
	//오버랩시 트리거 발동용 박스
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Floor Switch")
	class UBoxComponent* TriggerBoX;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Floor Switch")
	UStaticMeshComponent* FloorSwitch;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Floor Switch")
	UStaticMeshComponent* Door;

	//최초 로케이션
	UPROPERTY(BlueprintReadWrite,Category="Floor Switch")
	FVector InitialDoorLocation;

	//최초 로케이션
	UPROPERTY(BlueprintReadWrite,Category="Floor Switch")
	FVector InitialSwitchLocation;

	FTimerHandle SwitchHandle;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Floor Switch")
	float Switchtime;
	
	void CloseDoor();

	bool bCharacterOnSwitch;

	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor ,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent,Category="Floor Switch")
	void RaiseDoor();

	UFUNCTION(BlueprintImplementableEvent,Category="Floor Switch")
	void LowerDoor();

	UFUNCTION(BlueprintImplementableEvent,Category="Floor Switch")
	void RaiseFloorSwitch();

	UFUNCTION(BlueprintImplementableEvent,Category="Floor Switch")
	void LowerFloorSwitch();

	UFUNCTION(BlueprintCallable,Category="Floor Switch")
	void UpdateDoorLocation(float z);

	UFUNCTION(BlueprintCallable,Category="Floor Switch")
	void UpdateSwitchLocation(float z);
};
