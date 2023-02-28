// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PingPongBall.generated.h"


UCLASS()
class PINGPONG_API APingPongBall : public AActor
{
	GENERATED_BODY()
private:
	FVector StartingVector;
	
protected:
	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category="Components")
	class USphereComponent* BallCollision;
	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category="Components")
	UStaticMeshComponent* BallMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSoftObjectPtr<UMaterial> BallMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Ball Params")
	float MovementSpeed = 100;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category="Ball Params")
	UParticleSystem* HitSFX;

	FString PathToExplosionSFX = "/Game/StarterContent/Particles/P_Explosion.P_Explosion";
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	bool bIsMoving = false;


public:
	bool IsMoving() const
	{
		return bIsMoving;
	}

protected:
	UPROPERTY(Replicated)
	int32 ScoringPower = 1;
	
public:	
	// Sets default values for this actor's properties
	APingPongBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Move(float DeltaTime);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartMove();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StopMove();

	UFUNCTION(Server, Reliable)
	void Server_ResetBall();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_HitEffect();

	template<class T>
	T* LoadAsset(TSoftObjectPtr<T>& AssetRef);
	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void StartMove();

	UFUNCTION(BlueprintCallable)
	void StopMove();

	int32 GetAccumulatedScore() const {return ScoringPower;};

	UFUNCTION(BlueprintCallable)
	void ResetBall();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

};



