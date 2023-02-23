// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PingPongPlatform.generated.h"

UCLASS()
class PINGPONG_API APingPongPlatform : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class UBoxComponent* CollisionBox;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* PlatformBody;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed = 100.f;

	float currDeltaTime;
	
public:	
	// Sets default values for this actor's properties
	APingPongPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float AxisValue);

};
