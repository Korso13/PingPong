// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PingPongGate.generated.h"
UENUM()
enum class EPlayerID { Player1, Player2 };

UCLASS()
class PINGPONG_API APingPongGate : public AActor
{
	GENERATED_BODY()
protected:

	UPROPERTY(Replicated)
	int32 PlayerScore = 0;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Components")
	class UBoxComponent* GateScoringCollision;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Components")
	class UStaticMeshComponent* GateBody;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	EPlayerID ScoringPlayer;
	
public:	
	// Sets default values for this actor's properties
	APingPongGate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_ScoreHit(class APingPongBall* HittingBall);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetPlayerScore() const {return PlayerScore;};

	void ResetPlayerScore() {PlayerScore = 0;};

	UFUNCTION()
	void SetScoringPlayer(EPlayerID InPlayerID) {ScoringPlayer = InPlayerID;};

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EPlayerID GetScoringPlayer() const {return ScoringPlayer;};
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
