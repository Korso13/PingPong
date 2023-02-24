// Fill out your copyright notice in the Description page of Project Settings.


#include "PingPongGate.h"

#include "PingPongBall.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APingPongGate::APingPongGate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GateScoringCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Gate Collision"));
	SetRootComponent(GateScoringCollision);
	GateScoringCollision->OnComponentBeginOverlap.AddDynamic(this, &APingPongGate::OnComponentBeginOverlap);
	GateScoringCollision->SetIsReplicated(true);

	GateBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gate Body"));
	GateBody->SetupAttachment(RootComponent);

	//SetReplicates(true);
	bReplicates = true;
}

// Called when the game starts or when spawned
void APingPongGate::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APingPongGate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APingPongGate::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APingPongGate, PlayerScore);
	DOREPLIFETIME(APingPongGate, ScoringPlayer);
}

//Register scoring hit via overlap with Scoring collision box
void APingPongGate::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("Ball overlapped scoring collision box"));
	
	APingPongBall* Ball = Cast<APingPongBall>(OtherActor);
	if(!Ball)
	{
		return;
	}

	//Call score increase on serverside
	Server_ScoreHit(Ball);
}

//Server-side score registering
void APingPongGate::Server_ScoreHit_Implementation(APingPongBall* HittingBall)
{
	//Increase player's score on teh scoring gate (the one that the other player protects)
	PlayerScore += HittingBall->GetAccumulatedScore();

	//Drop the ball fresh into field's center
	HittingBall->ResetBall();

	//UE_LOG(LogTemp, Warning, TEXT("Ball scored hit with %s"), *this->GetName());
}
