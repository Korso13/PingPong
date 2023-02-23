// Fill out your copyright notice in the Description page of Project Settings.


#include "PingPongPlayerController.h"

#include "PingPongPlatform.h"

APingPongPlayerController::APingPongPlayerController()
{
	SetReplicates(true);
}

void APingPongPlayerController::SetStartTransform(FTransform PlayerTransform)
{
	StartTransform = PlayerTransform;
}

void APingPongPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("Move", this, &APingPongPlayerController::MoveRight);
}

void APingPongPlayerController::MoveRight(float AxisValue)
{
	if(AxisValue != 0)
	{
		Server_MoveRight(AxisValue);
	}
}

bool APingPongPlayerController::Server_MoveRight_Validate(float AxisValue)
{
	return true;
}

void APingPongPlayerController::Server_MoveRight_Implementation(float AxisValue)
{
	if(Platform)
	{
		Platform->Server_MoveRight(AxisValue);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PP_PlayerCOntroller: Platform not spawned!"));
	}
}

bool APingPongPlayerController::Init_Validate()
{
	return true;
}

void APingPongPlayerController::Init_Implementation()
{
	if(Platform)
	{
		Platform->Destroy();
	}

	SpawnPlatform(PlatformClass);
}

bool APingPongPlayerController::SpawnPlatform_Validate(TSubclassOf<APingPongPlatform> SpawnPlatformClass)
{
	return PlatformClass != NULL;
}

void APingPongPlayerController::SpawnPlatform_Implementation(TSubclassOf<APingPongPlatform> SpawnPlatformClass)
{
	Platform = dynamic_cast<APingPongPlatform*>(GetWorld()->SpawnActor<APingPongPlatform>(PlatformClass));
	if(Platform)
	{
		Platform->SetActorLocation(StartTransform.GetLocation());
		Platform->SetActorRotation(StartTransform.GetRotation());
	}
}




