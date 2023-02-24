// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PingPongGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PINGPONG_API APingPongGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	class APingPongPlayerController* Player1 = NULL;
	UPROPERTY()
	APingPongPlayerController* Player2 = NULL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class APlayerStart* PlayerStart1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	APlayerStart* PlayerStart2;

	/** The gate that the player must hit with a ball to score (not defend!). E.g. The gate on teh opposite side of the spawn point */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class APingPongGate* Player1ScoringGate;

	/** The gate that the player must hit with a ball to score (not defend!). E.g. The gate on teh opposite side of the spawn point */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class APingPongGate* Player2ScoringGate;
	
public:
	APingPongGameModeBase();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	
};
