// Copyright Epic Games, Inc. All Rights Reserved.


#include "PingPongGameModeBase.h"

#include "PingPongBall.h"
#include "PingPongGate.h"
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

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APingPongBall::StaticClass(), FoundActors);
	if(FoundActors.Num() != 0)
	{
		PingPongBall = Cast<APingPongBall>(FoundActors[0]);
		if(PingPongBall)
			PingPongBall->StopMove();
	}
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

	//Setting players' scoring gates if already not set in BP
	if(GetWorld() && (!Player1ScoringGate || !Player2ScoringGate))
	{
		TArray<AActor*> FoundGates;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APingPongGate::StaticClass(), FoundGates);
		if(FoundGates.Num() > 1)
		{
			float Distance1 = FoundGates[0]->GetDistanceTo(PlayerStart1);
			float Distance2 = FoundGates[0]->GetDistanceTo(PlayerStart2);
			
			//The furthest gate is the one the player has to score with a hit
			if(Distance1 > Distance2)
			{
				Player1ScoringGate = dynamic_cast<APingPongGate*>(FoundGates[0]);
				Player1ScoringGate->SetScoringPlayer(EPlayerID::Player1);
				Player2ScoringGate = dynamic_cast<APingPongGate*>(FoundGates[1]);
				Player2ScoringGate->SetScoringPlayer(EPlayerID::Player2);
			}
			else
			{
				Player1ScoringGate = dynamic_cast<APingPongGate*>(FoundGates[1]);
				Player1ScoringGate->SetScoringPlayer(EPlayerID::Player1);
				Player2ScoringGate = dynamic_cast<APingPongGate*>(FoundGates[0]);
				Player2ScoringGate->SetScoringPlayer(EPlayerID::Player2);
			}
		}
	}
	else if(Player1ScoringGate && Player2ScoringGate)
	{
		Player1ScoringGate->SetScoringPlayer(EPlayerID::Player1);
		Player2ScoringGate->SetScoringPlayer(EPlayerID::Player2);
	}

	
	
	//local vars for spawning newly joined player later in this function
	APingPongPlayerController* currPlayer;
	APlayerPawn* currPawn;
	APlayerStart* currSpawnPoint;
	
	//Appointing newly-joined player to one of Player slots in GameMode
	if(!Player1 && PlayerStart1)
	{
		Player1 = Cast<APingPongPlayerController>(NewPlayer);
		if(Player1)
		{
			Player1->SetPlayerID(EPlayerID::Player1);
			currSpawnPoint = PlayerStart1;
			currPlayer = Player1;
			UE_LOG(LogTemp, Warning, TEXT("PingPongGM: Player1 joined"));
		}
		else
		{
			return;
		}
	}
	else if(!Player2 && PlayerStart2)
	{
		Player2 = Cast<APingPongPlayerController>(NewPlayer);
		if(Player2)
		{
			Player2->SetPlayerID(EPlayerID::Player2);
			currPlayer = Player2;
			currSpawnPoint = PlayerStart2;
			UE_LOG(LogTemp, Warning, TEXT("PingPongGM: Player2 joined"));

			if(PingPongBall)
				PingPongBall->StartMove();
		}
		else
		{
			return;
		}
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
	if(currPlayer)
	{
		currPawn = Cast<APlayerPawn>(currPlayer->GetPawn());

		FActorSpawnParameters Params;
		Params.Owner = currPlayer;
	
		if(!currPawn)
		{
			currPawn = GetWorld()->SpawnActor<APlayerPawn>(DefaultPawnClass, Params);
		}
	}
	else
	{
		return;
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

void APingPongGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if(Player1 == Exiting)
	{
		Player1 = nullptr;
	}
	else
	{
		Player2 = nullptr;
	}

	if(PingPongBall)
	{
		PingPongBall->StopMove();
		PingPongBall->ResetBall();
	}

	if(Player1ScoringGate)
		Player1ScoringGate->ResetPlayerScore();
	if(Player2ScoringGate)
		Player2ScoringGate->ResetPlayerScore();
}

