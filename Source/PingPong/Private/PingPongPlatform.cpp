// Fill out your copyright notice in the Description page of Project Settings.


#include "PingPongPlatform.h"

#include "Components/BoxComponent.h"

// Sets default values
APingPongPlatform::APingPongPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision box"));
	RootComponent = CollisionBox;

	PlatformBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform Body"));
	PlatformBody->SetupAttachment(RootComponent);
	PlatformBody->SetIsReplicated(true);

	SetReplicates(true);
    // ReSharper disable once CppVirtualFunctionCallInsideCtor
    SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void APingPongPlatform::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APingPongPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	currDeltaTime = DeltaTime;
}

void APingPongPlatform::Server_MoveRight_Implementation(float AxisValue)
{
	FVector currLocation = GetActorLocation();
	FVector newLocation = currLocation + GetActorRightVector() * MaxSpeed * AxisValue * currDeltaTime;
	if(!SetActorLocation(newLocation, true))
	{
		SetActorLocation(currLocation);
	}
}

bool APingPongPlatform::Server_MoveRight_Validate(float AxisValue)
{
	return true;
}

