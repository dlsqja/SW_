// Fill out your copyright notice in the Description page of Project Settings.


#include "MainyPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Framework/MultiBox/ToolMenuBase.h"
#include "Kismet/GameplayStatics.h"

void AMainyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//기본위젯
	if(HUDOverlayasset)
	{
		HUDOverlay=CreateWidget<UUserWidget>(this,HUDOverlayasset);
	}

	HUDOverlay->AddToViewport();
	HUDOverlay->SetVisibility(ESlateVisibility::Visible);

	//적 체력바
	if(WEnemyHealthBar)
	{
		EnemyHealthBar=CreateWidget<UUserWidget>(this,WEnemyHealthBar);
		if(EnemyHealthBar)
		{
			EnemyHealthBar->AddToViewport();
			EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}
		FVector2D Vec(0.f,0.f);
		EnemyHealthBar->SetAlignmentInViewport(Vec);
	}
	bEnemyHealthBarVisible=false;
	
	EnemyLocation=FVector(0.f);

	//퍼즈메뉴
	if(WPauseMenu)
	{
		PauseMenu=CreateWidget<UUserWidget>(this,WPauseMenu);
		if(PauseMenu)
		{
			PauseMenu->AddToViewport();
			PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	bPauseMenuVisible=false;

}
void AMainyPlayerController::DisplayEnemyHealthBar()
{
	if(EnemyHealthBar)
	{
		bEnemyHealthBarVisible=true;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
		
	}
}
void AMainyPlayerController::RemoveEnemyHealthBar()
{
	if(EnemyHealthBar)
	{
		bEnemyHealthBarVisible=false;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
		
	}
}

void AMainyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(EnemyHealthBar)
	{
		FVector2D TargetVector;
		ProjectWorldLocationToScreen(EnemyLocation,TargetVector);
		TargetVector.X-=100.f;
		TargetVector.Y-=120.f;
	
		FVector2D SizeInViewport = FVector2D(300.f,25.f);
		
		EnemyHealthBar->SetPositionInViewport(TargetVector);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
		
	}
	
}

void AMainyPlayerController::DisplayPauseMenu_Implementation()
{
	if(PauseMenu)
	{
		FInputModeUIOnly UIMode;
		SetInputMode(UIMode);
		
		bShowMouseCursor=true;
		bPauseMenuVisible=true;
		PauseMenu->SetVisibility(ESlateVisibility::Visible);
	}
}
void AMainyPlayerController::RemovePauseMenu_Implementation()
{
	if(PauseMenu)
	{
		FInputModeGameOnly GameMode;
		SetInputMode(GameMode);
		
		bShowMouseCursor=false;
		bPauseMenuVisible=false;
	}
}

void AMainyPlayerController::PauseMenuToggle()
{
     if(bPauseMenuVisible)
     {
     	RemovePauseMenu();
     }
     else
     {
     	DisplayPauseMenu();
     }
	
}