// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreWidget.h"

#include "PingPongBall.h"
#include "PingPongGate.h"
#include "PingPongPlayerController.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

//Setting up widget: Getting owning player, in-game gates, putting them in Gates array to get info on the scoring for both players
void UScoreWidget::Init()
{
	OwningPlayer = Cast<APingPongPlayerController>(GetPlayerContext().GetPlayerController());
	
	if(GetWorld() && OwningPlayer)
	{
		//Finding the gates
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APingPongGate::StaticClass(), FoundActors);

		if(FoundActors.Num() == 2)
		{
			//casting found actors to gates
			APingPongGate* Gate1 = dynamic_cast<APingPongGate*>(FoundActors[0]);
			APingPongGate* Gate2 = dynamic_cast<APingPongGate*>(FoundActors[1]);

			if(Gate1 && Gate2)
			{
				Gates.Empty();
				//checking if the first gate is teh scoring gate of the player owning this widget. If true, we but it first on Gates array
				if(Gate1->GetScoringPlayer() == OwningPlayer->PlayerID)
				{
					Gates.Add(Gate1);
					Gates.Add(Gate2);
				}
				else //otherwise, we put the second gate first on out Gates array as it will be the widget-owning player's scoring gate
				{
					Gates.Add(Gate2);
					Gates.Add(Gate1);
				}
			}
		}

		FoundActors.Empty();
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APingPongBall::StaticClass(), FoundActors);
		if(FoundActors.Num() != 0)
		{
			Ball = Cast<APingPongBall>(FoundActors[0]);
		}
	}
}

void UScoreWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Init();
}

void UScoreWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(OwningPlayer && PlayerScore && EnemyScore && BallScore && Ball && (Gates.Num() == 2))
	{
		FString String;

		//Player's gate is always the first value in Gates array
		String.AppendInt(Gates[0]->GetPlayerScore());
		PlayerScore->SetText(FText::FromString(String));
		String.Empty();
		
		//Enemy gate is always second value in Gates array
		String.AppendInt(Gates[1]->GetPlayerScore());
		EnemyScore->SetText(FText::FromString(String));
		String.Empty();

		//Get ball's accumulated power
		String.AppendInt(Ball->GetAccumulatedScore());
		BallScore->SetText(FText::FromString(String));
		String.Empty();

		bShow2PlayerWaitNotification = !Ball->IsMoving();
	}
	else //called in case widget has not initialized properly yet
	{
		Init();
	}
}

/*void UScoreWidget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UScoreWidget, bShow2PlayerWaitNotification);
}*/
