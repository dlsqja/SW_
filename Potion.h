// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Potion.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT4_API APotion : public AItem
{
	GENERATED_BODY()


public:
	
	APotion();

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Heal")
	float HealAmount;
	
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent ,AActor* OtherActor ,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
};
