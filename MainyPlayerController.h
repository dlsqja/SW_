// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT4_API AMainyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	//UMG 참조
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Widget")
	TSubclassOf<UUserWidget> HUDOverlayasset;

	//위젯 생성수 위젯을 담을 변수
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Widget")
	UUserWidget* HUDOverlay;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Widget")
	TSubclassOf<UUserWidget> WEnemyHealthBar;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Widget")
	UUserWidget* EnemyHealthBar;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Widget")
	TSubclassOf<UUserWidget> WPauseMenu;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Widget")
	UUserWidget* PauseMenu;

	bool bEnemyHealthBarVisible;
	bool bPauseMenuVisible;

	void DisplayEnemyHealthBar();
	void RemoveEnemyHealthBar();

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable,Category="HUD")
	void DisplayPauseMenu();
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable,Category="HUD")
	void RemovePauseMenu();
	
	void PauseMenuToggle();

	FVector EnemyLocation;
	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
};
