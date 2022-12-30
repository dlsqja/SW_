// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorSwitch.h"


#include "Components/BoxComponent.h"
#include "PhysicsEngine/PhysicsSettings.h"

// Sets default values
AFloorSwitch::AFloorSwitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBoX=CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));
	RootComponent = TriggerBoX;
	
	FloorSwitch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorSwitch"));
	FloorSwitch->SetupAttachment(GetRootComponent());

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(GetRootComponent());

	TriggerBoX->OnComponentBeginOverlap.AddDynamic(this,&AFloorSwitch::OnOverlapBegin);
	TriggerBoX->OnComponentEndOverlap.AddDynamic(this,&AFloorSwitch::OnEndOverlap);
	
	//트리거 박스 세팅
	TriggerBoX->SetCollisionEnabled(ECollisionEnabled::QueryOnly);//겹침만 허용
	TriggerBoX->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);//콜리젼 객체 설정
	TriggerBoX->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);//모든 채널의 충돌 무시
	TriggerBoX->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);//폰에 대한 콜리전 설정 세팅

	TriggerBoX->SetBoxExtent(FVector(62.f,62.f,32.f));

	Switchtime=2.f;//타이머 시간 설정

	bCharacterOnSwitch=false;
	
}

// Called when the game starts or when spawned
void AFloorSwitch::BeginPlay()
{
	Super::BeginPlay();

	InitialDoorLocation=Door->GetComponentLocation();
	InitialSwitchLocation=FloorSwitch->GetComponentLocation();

}

// Called every frame
void AFloorSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFloorSwitch::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	bCharacterOnSwitch=true;
	RaiseDoor();
	LowerFloorSwitch();
	
}

void AFloorSwitch::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor ,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bCharacterOnSwitch=false;
	//GetWorldTimerManager().SetTimer(SwitchHandle,this,&AFloorSwitch::CloseDoor,2.0f);
	CloseDoor();
}


void AFloorSwitch::UpdateDoorLocation(float z)
{
	FVector Newvector=InitialDoorLocation;
	Newvector.Z-=z;
	Door->SetWorldLocation(Newvector);
}

void AFloorSwitch::UpdateSwitchLocation(float z)
{
	FVector Newvector=InitialSwitchLocation;
	Newvector.Z+=z;
	FloorSwitch->SetWorldLocation(Newvector);
	
}

void AFloorSwitch::CloseDoor()
{
	if(!bCharacterOnSwitch)
	{
		LowerDoor();
		RaiseFloorSwitch();
	}
}