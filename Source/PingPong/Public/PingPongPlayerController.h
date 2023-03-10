// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PingPongGate.h"
#include "GameFramework/PlayerController.h"
#include "PingPongPlayerController.generated.h"
/**
 * 
 */
UCLASS()
class PINGPONG_API APingPongPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	FTransform StartTransform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class APingPongPlatform> PlatformClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class UScoreWidget> WidgetClass;
	
	UPROPERTY()
	APingPongPlatform* Platform;
	
	UPROPERTY()
	class UScoreWidget* ScoreWidget;
	
public:
	
	UPROPERTY()
	EPlayerID PlayerID;
	
public:

	APingPongPlayerController();
	
	UFUNCTION()
	void SetStartTransform(FTransform PlayerTransform);

	UFUNCTION(Server, Reliable, WithValidation)
	void Init();

	UFUNCTION(Server, Reliable, WithValidation)
	void SpawnPlatform(TSubclassOf<APingPongPlatform> SpawnPlatformClass);
	
	UFUNCTION(Client, Reliable)
	void StartScoreHUD();

	void SetPlayerID(EPlayerID InID);
	
	virtual void SetupInputComponent() override;

protected:
	UFUNCTION()
	void MoveRight(float AxisValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float AxisValue);

	virtual void BeginPlay() override;
};
