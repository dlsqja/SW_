// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

UCLASS()
class PROJECT4_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPlatform();

	//메쉬
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Platform")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	FVector StartPoint;

	UPROPERTY(EditAnywhere,meta = (MakeEditWidget="true"))
	FVector EndPoint;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Platform")
	float Interspeed;

	FTimerHandle InterpTimer;

	float Intertime;
	
	bool bInterping;

	float Distance;

	
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ToggleInterpint();

	void SwapVectors(FVector& VectorOne,FVector& VectorTwo);
};
