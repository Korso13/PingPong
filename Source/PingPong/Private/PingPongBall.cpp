// Fill out your copyright notice in the Description page of Project Settings.


#include "PingPongBall.h"

#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

#include "Particles/ParticleSystem.h"

// Sets default values
APingPongBall::APingPongBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BallCollision = CreateDefaultSubobject<USphereComponent>("Collision");
	SetRootComponent(BallCollision);
	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>("Ball's body");
	BallMesh->SetupAttachment(RootComponent);

	BallMesh->SetIsReplicated(true);
	//SetReplicates(true);
	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void APingPongBall::BeginPlay()
{
	Super::BeginPlay();

	StartingVector = GetActorLocation();

	BallMesh->SetMaterial(0, LoadAsset<UMaterial>(BallMaterial));
}

// Called every frame
void APingPongBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(GetNetMode() != ENetMode::NM_Client && bIsMoving)
	{
		Server_Move(DeltaTime);
	}
}

//---------------------------------------------------------------------------------------------------

void APingPongBall::ResetBall()
{
	Server_ResetBall();
}

void APingPongBall::Server_ResetBall_Implementation()
{
	SetActorLocation(StartingVector);
	FRotator RandRotation = GetActorRotation();
	RandRotation.Yaw = FMath::RandRange(-360, 360);
	SetActorRotation(RandRotation);
	ScoringPower = 1;
}

//---------------------------------------------------------------------------------------------------

void APingPongBall::StartMove()
{
	Server_StartMove();
	UE_LOG(LogTemp, Warning, TEXT("Ball: start move"));
}

void APingPongBall::Server_StartMove_Implementation()
{
	bIsMoving = true;
	UE_LOG(LogTemp, Warning, TEXT("Ball: Server start move"));
}

bool APingPongBall::Server_StartMove_Validate()
{
	return true;
}

void APingPongBall::StopMove()
{
	Server_StopMove();
	UE_LOG(LogTemp, Warning, TEXT("Ball: stop move"));
}

void APingPongBall::Server_StopMove_Implementation()
{
	bIsMoving = false;
	UE_LOG(LogTemp, Warning, TEXT("Ball: Server stop move"));
}

bool APingPongBall::Server_StopMove_Validate()
{
	return true;
}

//---------------------------------------------------------------------------------------------------

void APingPongBall::Server_Move_Implementation(float DeltaTime)
{
	//Calculate new potential location for the ball
	FVector forwardVector = GetActorForwardVector();
    FVector currLoc = GetActorLocation();
	//UE_LOG(LogTemp, Warning, TEXT("Ball's Z is %f"), currLoc.Z);
    FVector newLoc = currLoc + forwardVector * MovementSpeed * DeltaTime;
	newLoc.Z = StartingVector.Z;
    FHitResult hitResult;

	//check if the ball would collide with anything if moved to a new position
    if(!SetActorLocation(newLoc, true, &hitResult))
    {
    	//UE_LOG(LogTemp, Warning, TEXT("Ball %s Collided with %s"), *GetName(), *hitResult.GetActor()->GetName());

    	//get current movement vector, normalize it for further operations
    	FVector moveVector = forwardVector;
	    moveVector.Normalize();

    	//Determine "reset" location - will place projectile there after turning it for a new movement vector
    	FVector resetPosition = currLoc + hitResult.ImpactNormal * 5;

    	
			//Debug arrows
    		DrawDebugDirectionalArrow(GetWorld(), newLoc, newLoc + moveVector * 300,
		    30, FColor::Yellow, true, 3.f, 0, 3);
		    FVector impactCorrection = hitResult.ImpactPoint +
		    hitResult.ImpactNormal * 300;
		    DrawDebugDirectionalArrow(GetWorld(), hitResult.ImpactPoint,
		    hitResult.ImpactPoint + hitResult.ImpactNormal * 300, 30, FColor::Blue, true, 3, 0,
		    3);
    	
		//get angle between movement vector and hit location's normal
    	float AimAtAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(moveVector, hitResult.ImpactNormal)));

    	//rotate the current movement vector to change projectile's movement direction (via rotation later)
    	moveVector = moveVector.RotateAngleAxis(AimAtAngle*2, FVector(0,0,1));
		//get new movement vector (used to get rotation value for projectile's actor)
    	FVector newTargetMove = newLoc + moveVector * 300;
		newTargetMove.Z = currLoc.Z;

    		//Debug arrows
    		DrawDebugDirectionalArrow(GetWorld(), newLoc, newTargetMove, 30,
			FColor::Cyan, true, 3.f, 0, 3);

    	//SetActorLocation(currLoc);
    	//Move ball back a bit after collision and to set a new coursse of movement
		SetActorLocation(resetPosition);

    	//Determine new rotation for ball based on new movement direction
    	FRotator currRotation = GetActorRotation();
		FRotator newRotation = UKismetMathLibrary::FindLookAtRotation(currLoc, newTargetMove);
		newRotation.Pitch = currRotation.Pitch;

    	bool NegativeFlag = FMath::RandBool();
		newRotation.Yaw = newRotation.Yaw + FMath::RandRange(90, 120) * (NegativeFlag ? 1 : -1);
		SetActorRotation(newRotation);
    	
		Multicast_HitEffect();
    	ScoringPower += 1;
	}
}

bool APingPongBall::Server_Move_Validate(float DeltaTime)
{
	return true;
}

//---------------------------------------------------------------------------------------------------
void APingPongBall::Multicast_HitEffect_Implementation()
{
	if(!HitSFX)
	{
		HitSFX = LoadObject<UParticleSystem>(NULL, *PathToExplosionSFX);
	}
	
	if(GetWorld() && HitSFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitSFX, GetActorLocation());
	}
}

//---------------------------------------------------------------------------------------------------

void APingPongBall::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APingPongBall, bIsMoving);
	DOREPLIFETIME(APingPongBall, ScoringPower);
}


//Asset-loading template function
template <class T>
T* APingPongBall::LoadAsset(TSoftObjectPtr<T>& AssetRef)
{
	if(AssetRef.IsPending())
	{
		const FSoftObjectPath& Ref = AssetRef.ToSoftObjectPath();
		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
		AssetRef = Cast<T>(StreamableManager.LoadSynchronous(Ref));
	}
	if(AssetRef.IsValid())
	{
		return AssetRef.Get();
	}
	else
	{
		return nullptr;
	}
}

