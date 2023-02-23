// Copyright Epic Games, Inc. All Rights Reserved.


#include "PingPongGameModeBase.h"

#include "PingPongPlayerController.h"
#include "PlayerPawn.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

APingPongGameModeBase::APingPongGameModeBase()
{
	DefaultPawnClass = APlayerPawn::StaticClass();
	PlayerControllerClass = APingPongPlayerController::StaticClass();
}

void APingPongGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void APingPongGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	//Getting player starts' pointers
	if(GetWorld() && (!PlayerStart1 || !PlayerStart2))
	{
		TArray<AActor*> FoundSpawnPoints;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundSpawnPoints);
		if(FoundSpawnPoints.Num() > 0)
			PlayerStart1 = dynamic_cast<APlayerStart*>(FoundSpawnPoints[0]);
		if(FoundSpawnPoints.Num() > 1)
			PlayerStart2 = dynamic_cast<APlayerStart*>(FoundSpawnPoints[1]);
	}

	//local vars for spawning newly joined player later in this function
	APingPongPlayerController* currPlayer;
	APlayerPawn* currPawn;
	APlayerStart* currSpawnPoint;
	
	//Appointing newly-joined player to one of Player slots in GameMode
	if(!Player1 && PlayerStart1)
	{
		Player1 = dynamic_cast<APingPongPlayerController*>(NewPlayer);
		currPlayer = Player1;
		currSpawnPoint = PlayerStart1;
		UE_LOG(LogTemp, Warning, TEXT("PingPongGM: Player1 joined"));
	}
	else if(!Player2 && PlayerStart2)
	{
		Player2 = dynamic_cast<APingPongPlayerController*>(NewPlayer);
		currPlayer = Player2;
		currSpawnPoint = PlayerStart2;
		UE_LOG(LogTemp, Warning, TEXT("PingPongGM: Player2 joined"));
	}
	else if(Player1 && Player2)
	{
		UE_LOG(LogTemp, Warning, TEXT("PingPongGM: All player slots are full"));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PingPongGM: Error! Insufficient number of player spawns!"));
		//can't properly allocate the player due to not finding enough Player Starts. About Login logic
		return;
	}

	//checking if new player has its pawn already spawned. If not, we spawn it
	currPawn = Cast<APlayerPawn>(currPlayer->GetPawn());
	if(!currPawn)
	{
		currPawn = GetWorld()->SpawnActor<APlayerPawn>(DefaultPawnClass);
	}

	//Place pawn on spawn point, assign it to player controller and initialize it
	if(currPawn && currSpawnPoint && currPlayer)
	{
		currPawn->SetActorLocation(currSpawnPoint->GetActorLocation());
		currPawn->SetActorRotation(currSpawnPoint->GetActorRotation());
		currPlayer->SetPawn(currPawn);

		currPlayer->SetStartTransform(currSpawnPoint->GetActorTransform());
		currPlayer->Init();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PingPongGM: failed to place new player pawn!"));
	}
}
