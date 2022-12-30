// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class PROJECT4_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SpawnBox")
	UBoxComponent* SpawnBox;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SpawnBox")
	TSubclassOf<AActor> Actor_1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SpawnBox")
	TSubclassOf<AActor> Actor_2;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SpawnBox")
	TSubclassOf<AActor> Actor_3;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="SpawnBox")
	TSubclassOf<AActor> Actor_4;

	TArray<TSubclassOf<AActor>> SpawnArray;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure,Category="Spawn")
	FVector GetSpawnPoint();

	UFUNCTION(BlueprintPure,Category="Spawn")
	TSubclassOf<AActor> GetSpawnActor();

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category="Spawn")
	void SpawnActor(UClass* ToSpawn, const FVector& Location);
};
