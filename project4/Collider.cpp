// Fill out your copyright notice in the Description page of Project Settings.


#include "Collider.h"

#include "ColliderMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ACollider::ACollider()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//AutoPossessPlayer = EAutoReceiveInput::Player0;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(SphereComponent);
	SphereComponent->InitSphereRadius(40.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Pawn"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetRelativeLocation(FVector(0.f,0.f,-40.f));

	//mesh 정보를 얻어옴
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComponentAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	if(MeshComponentAsset.Succeeded())
	{
		MeshComponent->SetStaticMesh(MeshComponentAsset.Object);
		MeshComponent->SetWorldScale3D(FVector(0.8f,0.8f,0.8f));
	}

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->SetRelativeRotation(FRotator(-60.0f,0.f,0.f));
	SpringArm->TargetArmLength= 400.0f;
	SpringArm->bEnableCameraLag=true;
	SpringArm->CameraLagSpeed=5.0f;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm,USpringArmComponent::SocketName);

	OurMovementComponent = CreateDefaultSubobject<UColliderMovementComponent>(TEXT("OurMovementComponent"));
	OurMovementComponent->UpdatedComponent= RootComponent;

	CameraInput=FVector2D(0.f,0.f);
	
}

// Called when the game starts or when spawned
void ACollider::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACollider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw +=CameraInput.X;

	FRotator TempRotate=SpringArm->GetComponentRotation();
	TempRotate.Pitch = FMath::Clamp(TempRotate.Pitch += CameraInput.Y,-80.f,0.f);
	
	SpringArm->SetWorldRotation(TempRotate);
	SetActorRotation(NewRotation);
}

// Called to bind functionality to input
void ACollider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis(TEXT("MoveForward"),this,&ACollider::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"),this,&ACollider::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("CameraPitch"),this,&ACollider::MousePitch);
	PlayerInputComponent->BindAxis(TEXT("CameraYaw"),this,&ACollider::MouseYaw);
}

void ACollider::MoveForward(float input)
{
	FVector Forward = GetActorForwardVector();
	AddMovementInput(Forward,5*input);
	// if (OurMovementComponent)
	// {
	// 	OurMovementComponent->AddInputVector(25*input*Forward);
	// 	UE_LOG(LogTemp,Warning,TEXT("%lf"),25*input);
	// }
}
void ACollider::MoveRight(float input)
{
	FVector Right = GetActorRightVector();
	AddMovementInput(Right,input*5);
	// if (OurMovementComponent)
	// {
	// 	OurMovementComponent->AddInputVector(5*input*Right);
	// 	UE_LOG(LogTemp,Warning,TEXT("%lf"),5*input);
	// }
	
}
void ACollider::MousePitch(float AxisValue)
{
	CameraInput.Y=AxisValue;
}
void ACollider::MouseYaw(float AxisValue)
{
	CameraInput.X=AxisValue;
}

UPawnMovementComponent* ACollider::GetMovementComponent() const
{
	return OurMovementComponent;
}